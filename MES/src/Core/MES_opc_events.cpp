#include "MES.h"

#define TOOLSET_BUFLEN 4

// Auxiliar Declarations
void chooseTools(std::vector<int16_t> &tools, uint16_t& piece_intermediate, uint64_t *tool_time, const TransformOrder &next_order);
void chooseToolSet(int16_t *tool_set, const std::vector<int16_t> &tools);
void chooseRoute(int16_t *route, const int16_t *tool_set, const std::vector<int16_t> &tools);

struct opc_transform
{
    uint16_t init_p;
    int16_t quantity;
    int16_t to_do;
    int16_t done;
    int16_t *tool_set;
    int16_t path[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint64_t tool_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    bool warehouse_intermediate;
    uint16_t piece_intermediate;
};

void MES::onSendTransform(int cell)
{
    if(!scheduler.hasTransform(cell))
        return;

    std::stringstream ss_node;
    ss_node << "orders_C" << cell;
    opc_transform order;
    std::shared_ptr<TransformOrder> next_order;
    if(cell == 1){
        if(scheduler.getTransformOrdersC1().empty()){
            return;
        }
        next_order = scheduler.getTransformOrdersC1()[0];
    }
    else{
        if(scheduler.getTransformOrdersC2().empty()){
            return;
        }
        next_order = scheduler.getTransformOrdersC2()[0];
    }
    //auto next_order = std::make_unique<TransformOrder>(111, 0, 1, P1, P7, 1, 30);
    if(next_order == nullptr){
        return;
    }
    order.init_p = next_order->getInitial();
    order.quantity = next_order->getQuantity();
    order.to_do = next_order->getQuantity();
    order.done = 0;

    // Get tools
    std::vector<int16_t> tools; tools.reserve(6);
    uint16_t piece_intermediate;
    chooseTools(tools, piece_intermediate, order.tool_time, *next_order);
    order.piece_intermediate = piece_intermediate;
    //MES_TRACE("Piece Init: {}", order.init_p);
    //MES_TRACE("Tool time: {}; {}; {}; {}; {}; {}; {};", order.tool_time[0], order.tool_time[1], order.tool_time[2], order.tool_time[3], order.tool_time[4], order.tool_time[5],  order.tool_time[6]);

    // Choose toolset
    int16_t tool_set[4] = {0,0,0,0};
    chooseToolSet(tool_set, tools);
    order.tool_set = tool_set;
    MES_TRACE("Tool_set: {}; {}; {}; {};", order.tool_set[0], order.tool_set[1], order.tool_set[2], order.tool_set[3]);

    // Choose route
    chooseRoute(order.path, tool_set, tools);
    //order.path = path;
    MES_TRACE("Path: {}; {}; {}; {}; {}; {}; {}; {};", order.path[0], order.path[1], order.path[2], order.path[3], order.path[4], order.path[5], order.path[6], order.path[7]);
    
    //Check for warehouse intermediate
    order.warehouse_intermediate = false;
    for(int i = 0; i<8; i++){
        if(order.path[i]==5){
            order.warehouse_intermediate = true;
        }
    }
    // MES_TRACE("Warehouse intermediate: {}", order.warehouse_intermediate);

    // Write to factory

    std::string node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].init_p"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.init_p);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].quantity"));
    int debug = fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.quantity);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].to_do"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.to_do);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].done"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.done);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].tool_set"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.tool_set, TOOLSET_BUFLEN);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].path"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.path, 8);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].tool_time"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.tool_time, 8);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].warehouse_intermidiate"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.warehouse_intermediate);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].piece_intermidiate"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.piece_intermediate);
}

struct opc_unload
{
    uint16_t type_t;
    int16_t dest;
    int16_t quant;
};

int writeUnload(OpcClient &opc_client, const opc_unload &order)
{
    std::string unload_node = "unload_orders";

    std::string node = std::move(std::string(OPC_GLOBAL_NODE_STR) + unload_node + std::string("[1].type_t"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.type_t)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + unload_node + std::string("[1].dest"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.dest)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + unload_node + std::string("[1].quant"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.quant)) return 0;

    return 1;
}

void MES::onSendUnload()
{
    if(!scheduler.hasUnload())
        return;

    std::shared_ptr<UnloadOrder> next_order = scheduler.getUnloadOrders()[0];
    opc_unload opc_u = {(uint16_t)next_order->getPiece(), (int16_t)next_order->getDest(), (int16_t)next_order->getQuantity()};

    if(!writeUnload(fct_client, opc_u))
        MES_ERROR("Could not send unload order.");

}

void MES::onLoadOrder(int conveyor)
{
    // Convert to a piece

    // Update Storage
}

void MES::onStartOrder(int cell)
{
    // Update scheduler by id
}

void MES::onFinishOrder(int cell)
{
    // Update scheduler by id
}

void MES::onUnloaded(dest_t dest)
{
    // Update stats 
}

void MES::onFinishProcessing()
{
    // Update stats
}

// ############################################ AUXILIAR FUNCTIONS #################################################
// #################################################################################################################

// get tools
void chooseTools(std::vector<int16_t>& tools, uint16_t& piece_intermediate, uint64_t *tool_time, const TransformOrder& next_order)
{
    piece_t piece_act = next_order.getInitial();
    int i = 0, intermediate = 0;
    while(piece_act != next_order.getFinal()){   
        switch (piece_act){     //Find next tool (incomplete)
        case P1:
            tools.push_back(0);
            tool_time[i+intermediate] = 15000;
            piece_act = P2;
            break;
        case P2:
            tools.push_back(1);
            tool_time[i+intermediate] = 15000;
            piece_act = P3;
            break;
        case P3:
            tools.push_back(2);
            tool_time[i+intermediate] = 15000;
            piece_act = P4;
            break;
        case P4:
            tools.push_back(0);
            tool_time[i+intermediate] = 15000;
            piece_act = P5;
            break;
        case P5:
            if(i==4){
                piece_intermediate = 5;
                tool_time[i+intermediate] = 0;
                intermediate++;
            }
            if(next_order.getFinal() == P9){
                tools.push_back(2);
                tool_time[i+intermediate] = 30000;
                piece_act = P9;
            }
            else{
                tools.push_back(1);
                tool_time[i+intermediate] = 30000;
                piece_act = P6;
            }
            break;
        case P6:
            if(i==4){
                piece_intermediate = 6;
                tool_time[i+intermediate] = 0;
                intermediate++;
            }
            if(next_order.getFinal() == P8){
                tools.push_back(0);
                tool_time[i+intermediate] = 15000;
                piece_act = P8;
            }
            else{
                tools.push_back(2);
                tool_time[i+intermediate] = 30000;
                piece_act = P7;
            }
            break;
        }
        i++;
    }
    
}

// choose toolset
void chooseToolSet(int16_t *tool_set, const std::vector<int16_t> &tools)
{
    for(int i = 0; i<tools.size(); i++){
        if(i <= 3){
            tool_set[i] = tools[i];
            continue;
        }
        for(int j = 0; j < TOOLSET_BUFLEN; j++){
            if(j == TOOLSET_BUFLEN-1){
                tool_set[j] = tools[i]; //in case there is a missing tool
            }
            if(tools[i]==tool_set[j]){
                break;
            }
        }
    }
}

// choose route
void chooseRoute(int16_t *route, const int16_t *tool_set, const std::vector<int16_t> &tools)
{
    int intermediate = 0;
    int mac_act = 0; //starts at warehouse
    for(int i = 0; i<tools.size(); i++){
        for(int j = mac_act; j < TOOLSET_BUFLEN; j++){   //piece can't go back to other conveyors
            if(tools[i] == tool_set[j]){
                route[i+intermediate] = (j+1);
                mac_act = j;
                break;
            }
            else{
                if(j == TOOLSET_BUFLEN-1){
                    route[i] = 5;
                    intermediate++;
                    // order.warehouse_intermediate = true;
                    mac_act = 0;
                    j = 0;
                }
            }
        }
    }
}

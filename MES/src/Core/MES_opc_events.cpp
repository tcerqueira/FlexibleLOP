#include "MES.h"

#define TOOLSET_BUFLEN 4

// Auxiliar Declarations
void chooseTools(std::vector<int16_t> &tools, uint16_t& piece_intermediate, std::vector<uint64_t> &tool_time, const TransformOrder &next_order);
void chooseToolSet(int16_t *tool_set, const std::vector<int16_t> &tools);
void chooseRoute(std::vector<int16_t> &route, const int16_t *tool_set, const std::vector<int16_t> &tools);

struct opc_order
{
    uint16_t init_p;
    int16_t quantity;
    int16_t to_do;
    int16_t done;
    int16_t *tool_set;
    std::vector<int16_t> path;
    std::vector<uint64_t> tool_time;
    bool warehouse_intermediate;
    uint16_t piece_intermediate;
};

void MES::onSendTransform(int cell)
{
    std::stringstream ss_node;
    ss_node << "orders_C" << cell;
    opc_order order;
    // TransformOrder* next_order = scheduler.getTransformOrders()[0];
    auto next_order = std::make_unique<TransformOrder>(111, 0, 3, P1, P8, 1, 30);
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
    //MES_TRACE("Piece Intermediate: {}", order.piece_intermediate);
    MES_TRACE("Tool time: {}; {}; {}; {}; {}; {}; {};", order.tool_time[0], order.tool_time[1], order.tool_time[2], order.tool_time[3], order.tool_time[4], order.tool_time[5],  order.tool_time[6]);

    // Choose toolset
    int16_t tool_set[4] = {0,0,0,0};
    chooseToolSet(tool_set, tools);
    order.tool_set = tool_set;
    MES_TRACE("Tool_set: {}; {}; {}; {};", order.tool_set[0], order.tool_set[1], order.tool_set[2], order.tool_set[3]);

    // Choose route
    std::vector<int16_t> path; path.reserve(8);
    chooseRoute(path, tool_set, tools);
    order.path = path;
    //MES_TRACE("Path: {}; {}; {}; {}; {}; {}; {}; {};", order.path[0], order.path[1], order.path[2], order.path[3], order.path[4], order.path[5], order.path[6], order.path[7]);
    
    //Check for warehouse intermediate
    order.warehouse_intermediate = false;
    for(int i = 0; i<order.path.size(); i++){
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
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), &order.path[0], 8);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].tool_time"));
    MES_TRACE(fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), &order.tool_time[0], 8));

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].warehouse_intermidiate"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.warehouse_intermediate);

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].piece_intermidiate"));
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.piece_intermediate);
}

void MES::onSendUnload()
{

}

void MES::onLoadOrder()
{

}

void MES::onStartOrder()
{

}

void MES::onFinishOrder()
{

}

void MES::onUnloaded()
{
    
}

void MES::onFinishProcessing()
{

}

// ############################################ AUXILIAR FUNCTIONS #################################################
// #################################################################################################################

// get tools
void chooseTools(std::vector<int16_t>& tools, uint16_t& piece_intermediate, std::vector<uint64_t> &tool_time, const TransformOrder& next_order)
{
    piece_t piece_act = next_order.getInitial();
    int i = 0;
    while(piece_act != next_order.getFinal()){   
        switch (piece_act){     //Find next tool (incomplete)
        case P1:
            tools.push_back(0);
            tool_time.push_back(15000);
            piece_act = P2;
            break;
        case P2:
            tools.push_back(1);
            tool_time.push_back(15000);
            piece_act = P3;
            break;
        case P3:
            tools.push_back(2);
            tool_time.push_back(15000);
            piece_act = P4;
            break;
        case P4:
            tools.push_back(0);
            tool_time.push_back(15000);
            piece_act = P5;
            break;
        case P5:
            if(i==4){
                piece_intermediate = 5;
                tool_time.push_back(0);
            }
            if(next_order.getFinal() == P9){
                tools.push_back(2);
                tool_time.push_back(30000);
                piece_act = P9;
            }
            else{
                tools.push_back(1);
                tool_time.push_back(30000);
                piece_act = P6;
            }
            break;
        case P6:
            if(i==4){
                piece_intermediate = 6;
                tool_time.push_back(0);
            }
            if(next_order.getFinal() == P8){
                tools.push_back(0);
                tool_time.push_back(15000);
                piece_act = P8;
            }
            else{
                tools.push_back(2);
                tool_time.push_back(30000);
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
void chooseRoute(std::vector<int16_t> &route, const int16_t *tool_set, const std::vector<int16_t> &tools)
{
    int mac_act = 0; //starts at warehouse
    for(int i = 0; i<tools.size(); i++){
        for(int j = mac_act; j < TOOLSET_BUFLEN; j++){   //piece can't go back to other conveyors
            if(tools[i] == tool_set[j]){
                route.push_back(j+1);
                mac_act = j;
                break;
            }
            else{
                if(j == TOOLSET_BUFLEN-1){
                    route.push_back(5);
                    // order.warehouse_intermediate = true;
                    mac_act = 0;
                    j = 0;
                }
            }
        }
    }
    route.push_back(0);
}

#include "MES.h"

#define TOOLSET_BUFLEN 4

// Auxiliar Declarations
void chooseTools(std::vector<int16_t> &tools, uint16_t& piece_intermediate, uint64_t *tool_time, const TransformOrder &next_order);
void chooseToolSet(int16_t *tool_set, const std::vector<int16_t> &tools);
void chooseRoute(int16_t *route, const int16_t *tool_set, const std::vector<int16_t> &tools);

struct opc_transform
{
    int16_t orderID;
    uint16_t init_p;
    int16_t quantity;
    int16_t to_do;
    int16_t done;
    int16_t* tool_set;
    int16_t path[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint64_t tool_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    bool warehouse_intermediate;
    uint16_t piece_intermediate;
};

int writeTransform(OpcClient &opc_client, opc_transform &order, int cell)
{
    std::stringstream ss_node;
    ss_node << "orders_C" << cell;

    std::string node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].init_p"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.init_p))   return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].orderID"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.orderID))  return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].quantity"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.quantity)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].to_do"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.to_do))    return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].done"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.done)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].tool_set"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.tool_set, TOOLSET_BUFLEN)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].path"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.path, 8))  return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].tool_time"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.tool_time, 8)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].warehouse_intermidiate"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.warehouse_intermediate))   return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].piece_intermidiate"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.piece_intermediate))   return 0;

    return 1;
}

void MES::onSendTransform(int cell)
{
    if(!scheduler.hasTransform(cell))
        return;

    opc_transform order;
    std::shared_ptr<TransformOrder> next_order;
    if(cell == 1){
        next_order = scheduler.getTransformOrdersC1()[0];
    }
    else{
        next_order = scheduler.getTransformOrdersC2()[0];
    }
    //auto next_order = std::make_unique<TransformOrder>(111, 0, 1, P1, P7, 1, 30);
    if(next_order == nullptr){
        return;
    }
    order.orderID = next_order->getId();
    order.init_p = next_order->getInitial();
    order.quantity = next_order->getQuantity();
    order.to_do = next_order->getQuantity();
    order.done = 0;

    // Get tools
    std::vector<int16_t> tools; tools.reserve(6);
    uint16_t piece_intermediate;
    chooseTools(tools, piece_intermediate, order.tool_time, *next_order);
    order.piece_intermediate = piece_intermediate;
    
    // Choose toolset
    int16_t tool_set[4] = {0,0,0,0};
    chooseToolSet(tool_set, tools);
    order.tool_set = tool_set;

    // Choose route
    chooseRoute(order.path, tool_set, tools);
    
    //Check for warehouse intermediate
    order.warehouse_intermediate = false;
    for(int i = 0; i<8; i++){
        if(order.path[i]==5){
            order.warehouse_intermediate = true;
        }
    }

    // Write to factory
    writeTransform(fct_client, order, cell);
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
    std::shared_ptr<UnloadOrder> next_unload = scheduler.popUnload();
    if(next_unload == nullptr)
        return;

    opc_unload opc_u = {(uint16_t)next_unload->getPiece(), (int16_t)next_unload->getDest(), (int16_t)next_unload->getQuantity()};

    if(!writeUnload(fct_client, opc_u))
        MES_ERROR("Could not send unload order.");
}

void MES::onLoadOrder(piece_t piece)
{
    // Update Storage
    MES_TRACE("Piece {} loaded.", (int)piece);
    store.addCount(piece, 1);
}

void MES::onStartPiece(int cell)
{
    // Update scheduler by id
    std::stringstream ss_node;
    ss_node << OPC_GLOBAL_NODE_STR << "starting_piece_numberC" << cell;
    const std::string &str_node = ss_node.str();

    UA_Variant number_var;
    UA_Variant_init(&number_var);
    if(!fct_client.readValueInt16(UA_NODEID_STRING_ALLOC(4, str_node.c_str()), number_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_node);
    }
    int number = *(int*)number_var.data;
    MES_TRACE("Piece of order {} started on cell {}.", number, cell);
    scheduler.updatePieceStarted(cell, number);
    UA_Variant_clear(&number_var);
}

void MES::onFinishPiece(int cell)
{
    // Update scheduler by id
    std::stringstream ss_node;
    ss_node << OPC_GLOBAL_NODE_STR << "finishing_piece_numberC" << cell;
    const std::string &str_node = ss_node.str();

    UA_Variant number_var;
    UA_Variant_init(&number_var);
    if(!fct_client.readValueInt16(UA_NODEID_STRING_ALLOC(4, str_node.c_str()), number_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_node);
    }
    int number = *(int*)number_var.data;
    MES_TRACE("Piece of order {} finished on cell {}.", number, cell);
    scheduler.updatePieceFinished(cell, number);
    UA_Variant_clear(&number_var);
}

void MES::onUnloaded(dest_t dest)
{
    // Update stats
    std::stringstream ss_node;
    ss_node << OPC_GLOBAL_NODE_STR << "unloaded_PM" << (int)dest << "_type";
    const std::string &str_node = ss_node.str();

    UA_Variant type_var;
    UA_Variant_init(&type_var);
    if(!fct_client.readValueUInt16(UA_NODEID_STRING_ALLOC(4, str_node.c_str()), type_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_node);
    }
    piece_t unload_piece = (piece_t)(int)*(uint16_t*)type_var.data;
    MES_TRACE("Unloaded type {} on destination {}.", (int)unload_piece, (int)dest);
    factory.unloaded(unload_piece, dest);
    UA_Variant_clear(&type_var);
}

void MES::onFinishProcessing(int machine)
{
    // Update stats
    std::stringstream ss_type_node; ss_type_node << "machined_type" << "[" << machine << "]";
    std::stringstream ss_time_node; ss_time_node << "machined_time" << "[" << machine << "]";

    const std::string &str_type_node = ss_type_node.str();
    const std::string &str_time_node = ss_time_node.str();

    UA_Variant type_var, time_var;

    UA_Variant_init(&type_var);
    if(!fct_client.readValueUInt16(UA_NODEID_STRING_ALLOC(4, str_type_node.c_str()), type_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_type_node);
    }

    UA_Variant_init(&time_var);
    if(!fct_client.readValueInt16(UA_NODEID_STRING_ALLOC(4, str_time_node.c_str()), time_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_time_node);
    }

    piece_t machined_piece = (piece_t)(int)*(uint16_t*)type_var.data;
    unsigned int machined_time = (unsigned int)*(int16_t*)time_var.data;
    MES_TRACE("Machine {}: type {} and time {}.", machine, (int)machined_piece, machined_time);
    factory.machined(machine, machined_piece, machined_time);
    UA_Variant_clear(&type_var);
    UA_Variant_clear(&time_var);
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

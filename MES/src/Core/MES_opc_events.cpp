#include "MES.h"

#define TOOLSET_BUFLEN 4

// Auxiliar Declarations
void chooseTools(std::vector<int> &tools, const TransformOrder &next_order);
void chooseToolSet(int *tool_set, const std::vector<int> &tools);
void chooseRoute(std::vector<int> &route, const int *tool_set, const std::vector<int> &tools);

struct opc_order
{
    uint16_t init_p;
    int quantity;
    int to_do;
    int done;
    int *tool_set;
    std::vector<int> path;
    std::vector<int> tool_time;
    bool warehouse_intermediate;
    uint16_t piece_intermidiate;
};

void MES::onSendTransform(int cell)
{
    std::stringstream ss_node;
    ss_node << "orders_C" << cell;
    opc_order order;
    // TransformOrder* next_order = scheduler.getTransformOrders()[0];
    auto next_order = std::make_unique<TransformOrder>(111, 0, 3, P2, P4, 1, 30);
    if(next_order == nullptr){
        return;
    }
    order.init_p = next_order->getInitial();
    order.quantity = next_order->getQuantity();
    order.to_do = next_order->getQuantity();
    order.done = 0;

    // Get tools
    std::vector<int> tools; tools.reserve(6);
    chooseTools(tools, *next_order);
    //MES_TRACE("Tools: {}; {}; {};", tools[0], tools[1], tools[2]);

    // Choose toolset
    int tool_set[4] = {0,0,0,0};
    chooseToolSet(tool_set, tools);
    order.tool_set = tool_set;
    //MES_TRACE("Tool_set: {}; {}; {}; {};", order.tool_set[0], order.tool_set[1], order.tool_set[2], order.tool_set[3]);

    // Choose route
    std::vector<int> path; path.reserve(6);
    chooseRoute(path, tool_set, tools);
    order.path = path;
    //MES_TRACE("Path: {}; {}; {};", order.path[0], order.path[1], order.path[2]);


    uint16_t piece_init = 1;
    std::string node = std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].init_p");
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), piece_init);

    int16_t quantity = 5;
    node = std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].n_pieces");
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), quantity);

    /*orders_C1[1].init_p;
    orders_C1[1].warehouse_intermediate;
    orders_C1[1].quantity;
    orders_C1[1].to_do;
    orders_C1[1].tool_set[4];
    orders_C1[1].path[8];
    orders_C1[1].tool_time[8];*/
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
void chooseTools(std::vector<int>& tools, const TransformOrder& next_order)
{
    piece_t piece_act = next_order.getInitial();
    int i = 0;
    while(piece_act != next_order.getFinal()){
        if(i==4){
            //order.piece_intermediate = ;
        }
        
        switch (piece_act){     //Find next tool (incomplete)
        case P1:
            tools.push_back(0);
            // order.tool_time.push_back(15000);
            piece_act = P2;
            break;
        case P2:
            tools.push_back(1);
            // order.tool_time.push_back(15000);
            piece_act = P3;
            break;
        case P3:
            tools.push_back(2);
            // order.tool_time.push_back(15000);
            piece_act = P4;
            break;
        }
        i++;
    }
    
}

// choose toolset
void chooseToolSet(int *tool_set, const std::vector<int> &tools)
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
void chooseRoute(std::vector<int> &route, const int *tool_set, const std::vector<int> &tools)
{
    int mac_act = 0; //starts at warehouse
    for(int i = 0; i<tools.size(); i++){
        for(int j = mac_act; j < TOOLSET_BUFLEN; j++){   //piece can't go back to other conveyors
            if(tools[i] == tool_set[j]){
                route.push_back(j+1);
                mac_act = j+1;
                break;
            }
            else{
                if(j == TOOLSET_BUFLEN-1){
                    route.push_back(5);
                    // order.warehouse_intermediate = true;
                    mac_act = 0;
                }
            }
        }
    }
}

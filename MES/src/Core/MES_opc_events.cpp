#include "MES.h"

void MES::onSendTransform()
{
    TransformOrder next_order = Scheduler::getTransformOrders[1];

    // Get tools
    vector<int> tools;
    int piece_act = next_order.getInitial();

    switch (piece_act){     //Find next tool (incomplete)
        case 1:
            tools.push_back(0);
            piece_act = 2;
            break;
        case 2:
            tools.push_back(1);
            piece_act = 3;
            break;
        case 3:
            tools.push_back(2);
            piece_act = 4;
            break;
    }

    //Choose toolset
    int tool_set[4] = {0,0,0,0};
    for(int i = 0; i<tools.size(); i++){
        if(i <= 3){
            tool_set[i] = tools[i];
            continue;
        }
        for(int j = 0; j < 4, j++){
            if(j == 3){
                tool_set[j] = tools[i]; //in case there is a missing tool
            }
            if(tools[i]==tool_set[j]){
                break;
            }
        }
    }

    //Choose route
    vector<int> route;
    int mac_act = 0; //starts at warehouse
    for(int i = 0; i<tools.size(); i++){
        for(int j = mac_act; j < 4; j++){   //piece can't go back to other conveyors
            if(tools[i] == tool_set[j]){
                route.push_back(j+1);
                mac_act = j+1;
            }
            else{
                if(j == 3){
                    route.push_back(5);
                    mac_act = 0;
                }
            }
        }
    }

    uint16_t piece_init = 1;
    std::string node = std::string(OPC_GLOBAL_NODE_STR) + std::string("orders_C1[1].init_p");
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), piece_init);

    UA_Variant order_C1_quantity;
    UA_Variant_init(&order_C1_quantity);
    int16_t quantity = 5;
    UA_Variant_setScalar(&order_C1_quantity, &quantity, &UA_TYPES[UA_TYPES_INT16]);
    node = std::string(OPC_GLOBAL_NODE_STR) + std::string("orders_C1[1].n_pieces");
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order_C1_quantity);

    //fct_client;

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

#include "MES.h"

void MES::onSendTransform()
{
    UA_Variant order_C1_piece_init;
    UA_Variant_init(&order_C1_piece_init);
    uint16_t piece_init = 1;
    UA_Variant_setScalar(&order_C1_piece_init, &piece_init, &UA_TYPES[UA_TYPES_UINT16]);
    std::string node = std::string(OPC_GLOBAL_NODE_STR) + std::string("orders_C1[1].init_p");
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order_C1_piece_init);

    UA_Variant order_C1_quantity;
    UA_Variant_init(&order_C1_quantity);
    int16_t quantity = 5;
    UA_Variant_setScalar(&order_C1_quantity, &quantity, &UA_TYPES[UA_TYPES_INT16]);
    node = std::string(OPC_GLOBAL_NODE_STR) + std::string("orders_C1[1].n_pieces");
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order_C1_quantity);

    UA_Variant order_C1_clear_flag;
    UA_Variant_init(&order_C1_clear_flag);
    bool req_orderC1_flag = false;
    UA_Variant_setScalar(&order_C1_clear_flag, &req_orderC1_flag, &UA_TYPES[UA_TYPES_BOOLEAN]);
    node = std::string(OPC_GLOBAL_NODE_STR) + std::string("req_orderC1_flag");
    fct_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order_C1_clear_flag);

    //fct_client;

    /*orders_C1[1].init_p;
    orders_C1[1].n_pieces;
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

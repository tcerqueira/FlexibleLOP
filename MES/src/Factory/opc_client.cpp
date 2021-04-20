#include "opc_client.h"

opc_client::opc_client()
{
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    UA_Variant_init(&value);
}

int opc_client::connect(const char* endpoint)
{
    std::string url = std::string("opc.tcp://") + std::string(endpoint);
    connectionStatus = UA_Client_connect(client, url.c_str());
    if(connectionStatus != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return 0;
    }
    return 1;
}           

opc_client::~opc_client()
{
    UA_Variant_clear(&value);
    UA_Client_delete(client);
}

int opc_client::readvalue()
{
    connectionStatus = UA_Client_readValueAttribute(client, UA_NODEID_STRING(1, "the.answer"), &value);
    if(connectionStatus == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT32])) {
        printf("the value is: %i\n", *(UA_Int32*)value.data);
        return 1;
    }
    return 0;
}

int opc_client::writevalue(UA_NodeId nodeid, UA_Variant *newValue)
{
    connectionStatus = UA_Client_writeValueAttribute(client, nodeid, newValue);
}
#include "opc_client.h"

opc_client::opc_client()
{
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
}

int opc_client::connect(const char* endpoint)
{
    std::string url = std::string("opc.tcp://") + std::string(endpoint);
    connectionStatus = UA_Client_connect(client, url.c_str());
    return 1;
}           

opc_client::~opc_client()
{
    UA_Client_delete(client);
}

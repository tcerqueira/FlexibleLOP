#include "opc_client.h"

OpcClient::OpcClient()
{
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
}

int OpcClient::connect(const std::string &endpoint)
{
    std::string url = std::string("opc.tcp://") + endpoint;
    connectionStatus = UA_Client_connect(client, url.c_str());
    return 1;
}           

OpcClient::~OpcClient()
{
    UA_Client_delete(client);
}

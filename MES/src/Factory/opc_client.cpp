#include "opc_client.h"

OpcClient::OpcClient()
: isListening(false)
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

void OpcClient::startListening(int t_ms)
{
    isListening = true;
    while(isListening)
    {
        listeners[REQ_ORDER].at(0)({REQ_ORDER, t_ms});
        std::this_thread::sleep_for(std::chrono::milliseconds(t_ms));
    }
}

void OpcClient::stopListening()
{
    isListening = false;
}

void OpcClient::addListener(opc_evt_type type, evtHandler handler)
{
    listeners[type].push_back(handler);
}

OpcClient::~OpcClient()
{
    UA_Client_delete(client);
}

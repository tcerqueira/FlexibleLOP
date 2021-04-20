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
        // opc_evt<int> evt = {REQ_ORDER, t_ms};
        // TODO: check when events happen
        if(true)
            notify({REQ_ORDER, t_ms});
        if(true)
            notify({ORDER_BEGIN, t_ms});
        if(true)
            notify({ORDER_END, t_ms});
        std::this_thread::sleep_for(std::chrono::milliseconds(t_ms));
    }
}

void OpcClient::notify(opc_evt evt)
{
    for(auto listener : listeners[evt.type])
    {
        listener(evt);
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

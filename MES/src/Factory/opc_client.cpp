#include "opc_client.h"

OpcClient::OpcClient(const std::string& opc_endpoint)
: isListening(false), endpoint(opc_endpoint)
{
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
}

int OpcClient::connect()
{
    std::string url = std::string("opc.tcp://") + endpoint;
    connectionStatus = UA_Client_connect(client, url.c_str());
    if(connectionStatus != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return 0;
    }
    return 1;
}

void OpcClient::startListening(int t_ms)
{
    if(connectionStatus != UA_STATUSCODE_GOOD){
        return;
    }
    
    isListening = true;
    while(isListening)
    {
        for(NodeKey node_key : event_nodes)
        {
            if(checkFlag(UA_NODEID_STRING_ALLOC(node_key.name_space, node_key.id_str.c_str())))
                notify({node_key, 0});
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(t_ms));
    }
}

void OpcClient::notify(opc_evt evt)
{
    for(auto listener : listeners[evt.node])
    {
        listener(evt);
    }
}

void OpcClient::stopListening()
{
    isListening = false;
}

bool OpcClient::checkFlag(UA_NodeId node)
{
    UA_Variant flag;
    UA_Variant_init(&flag);
    if(!readValueBool(node, flag)){
        return false;
    }
    return *(bool*)flag.data;
}

void OpcClient::addListener(NodeKey type, evtHandler handler)
{
    listeners[type].push_back(handler);
    event_nodes.insert(type);
}

int OpcClient::readValueBool(UA_NodeId nodeid, UA_Variant &value)
{
    UA_StatusCode read_status = UA_Client_readValueAttribute(client, nodeid, &value);
    if(read_status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_BOOLEAN])) {
        return 1;
    };
    return 0;
}

int OpcClient::readValueInt16(UA_NodeId nodeid, UA_Variant &value)
{
    UA_StatusCode read_status = UA_Client_readValueAttribute(client, nodeid, &value);
    if(read_status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT16])) {
        return 1;
    };
    return 0;
}

int OpcClient::readValueInt32(UA_NodeId nodeid, UA_Variant &value)
{
    UA_StatusCode read_status = UA_Client_readValueAttribute(client, nodeid, &value);
    if(read_status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT32])) {
        return 1;
    };
    return 0;
}

int OpcClient::writeValue(UA_NodeId nodeid, UA_Variant &newValue)
{
    UA_StatusCode write_status = UA_Client_writeValueAttribute(client, nodeid, &newValue);
    if(write_status == UA_STATUSCODE_GOOD) {
        return 1;
    }
    return 0;
}

OpcClient::~OpcClient()
{
    UA_Client_delete(client);
}
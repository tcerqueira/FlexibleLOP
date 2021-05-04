#include "Opc_Client.h"

OpcClient::OpcClient(const std::string &opc_endpoint)
    : isListening(false), endpoint(opc_endpoint)
{
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
}

int OpcClient::connect()
{
    std::string url = std::string("opc.tcp://") + endpoint;
    connectionStatus = UA_Client_connect(client, url.c_str());
    if (connectionStatus != UA_STATUSCODE_GOOD)
    {
        UA_Client_delete(client);
        return 0;
    }
    return 1;
}

void OpcClient::startListening(int t_ms)
{
    if (connectionStatus != UA_STATUSCODE_GOOD)
    {
        return;
    }

    isListening = true;
    // polling loop
    while (isListening)
    {
        UA_NodeId flag_node;
        // for each subscribed event flag
        for (NodeKey node_key : event_nodes)
        {
            flag_node = UA_NODEID_STRING_ALLOC(node_key.name_space, node_key.id_str.c_str());
            if (checkFlag(flag_node))
            {
                notify({node_key, 0});
                clearFlag(flag_node);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(t_ms));
    }
}

void OpcClient::notify(opc_evt evt)
{
    for (auto listener : listeners[evt.node])
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
    if (!readValueBool(node, flag))
    {
        return false;
    }
    return *(bool *)flag.data;
}

void OpcClient::clearFlag(UA_NodeId node)
{
    writeValue(node, false);
}

void OpcClient::addListener(NodeKey type, evtHandler handler)
{
    listeners[type].push_back(handler);
    event_nodes.insert(type);
}

int OpcClient::readValueBool(UA_NodeId nodeid, UA_Variant &value)
{
    UA_StatusCode read_status = UA_Client_readValueAttribute(client, nodeid, &value);
    if (read_status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_BOOLEAN]))
    {
        return 1;
    };
    return 0;
}

int OpcClient::readValueInt16(UA_NodeId nodeid, UA_Variant &value)
{
    UA_StatusCode read_status = UA_Client_readValueAttribute(client, nodeid, &value);
    if (read_status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT16]))
    {
        return 1;
    };
    return 0;
}

int OpcClient::readValueInt32(UA_NodeId nodeid, UA_Variant &value)
{
    UA_StatusCode read_status = UA_Client_readValueAttribute(client, nodeid, &value);
    if (read_status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT32]))
    {
        return 1;
    };
    return 0;
}

int OpcClient::writeValue(UA_NodeId nodeid, UA_Variant &newValue)
{
    UA_StatusCode write_status = UA_Client_writeValueAttribute(client, nodeid, &newValue);
    if (write_status == UA_STATUSCODE_GOOD)
    {
        return 1;
    }
    return 0;
}

int OpcClient::writeValue(UA_NodeId nodeid, int16_t value)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setScalar(&var_value, &value, &UA_TYPES[UA_TYPES_INT16]);
    return writeValue(nodeid, var_value);
}

int OpcClient::writeValue(UA_NodeId nodeid, int32_t value)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setScalar(&var_value, &value, &UA_TYPES[UA_TYPES_INT32]);
    return writeValue(nodeid, var_value);
}

int OpcClient::writeValue(UA_NodeId nodeid, uint16_t value)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setScalar(&var_value, &value, &UA_TYPES[UA_TYPES_UINT16]);
    return writeValue(nodeid, var_value);
}

int OpcClient::writeValue(UA_NodeId nodeid, uint32_t value)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setScalar(&var_value, &value, &UA_TYPES[UA_TYPES_UINT32]);
    return writeValue(nodeid, var_value);
}

int OpcClient::writeValue(UA_NodeId nodeid, bool value)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setScalar(&var_value, &value, &UA_TYPES[UA_TYPES_BOOLEAN]);
    return writeValue(nodeid, var_value);
}

int OpcClient::writeValue(UA_NodeId nodeid, int16_t *value, int len)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setArray(&var_value, value, len, &UA_TYPES[UA_TYPES_INT16]);
    return writeValue(nodeid, var_value);
}

int OpcClient::writeValue(UA_NodeId nodeid, int32_t *value, int len)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setArray(&var_value, value, len, &UA_TYPES[UA_TYPES_INT32]);
    return writeValue(nodeid, var_value);
}

int OpcClient::writeValue(UA_NodeId nodeid, uint16_t *value, int len)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setArray(&var_value, value, len, &UA_TYPES[UA_TYPES_UINT16]);
    return writeValue(nodeid, var_value);
}

int OpcClient::writeValue(UA_NodeId nodeid, uint32_t *value, int len)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setArray(&var_value, value, len, &UA_TYPES[UA_TYPES_UINT32]);
    return writeValue(nodeid, var_value);
}

int OpcClient::writeValue(UA_NodeId nodeid, bool *value, int len)
{
    UA_Variant var_value;
    UA_Variant_init(&var_value);
    UA_Variant_setArray(&var_value, value, len, &UA_TYPES[UA_TYPES_BOOLEAN]);
    return writeValue(nodeid, var_value);
}



OpcClient::~OpcClient()
{
    UA_Client_delete(client);
}
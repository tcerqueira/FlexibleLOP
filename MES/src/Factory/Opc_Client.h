#pragma once

#include "l_comms.h"
#include <unordered_map>
#include <unordered_set>

#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>

// enum opc_evt_type {
//     REQ_ORDER, ORDER_BEGIN, ORDER_END, _END_EVT_
// };

struct NodeKey
{
    int name_space;
    std::string id_str;

    bool operator==(const NodeKey &other) const
    {
        return (name_space == other.name_space && id_str == other.id_str);
    }

    struct KeyHasher
    {
        std::size_t operator()(const NodeKey& key) const
        {
            std::size_t nsHash = std::hash<int>()(key.name_space);
            std::size_t idHash = std::hash<std::string>()(key.id_str);
            return nsHash ^ idHash;
        }
    };
};

struct opc_evt
{
    NodeKey node;
    int data; // temporary testing
};

class OpcClient
{
public:
    using evtHandler = std::function<void(opc_evt)>;

    OpcClient(const std::string& opc_endpoint);
    OpcClient(std::string&& opc_endpoint);
    ~OpcClient();
    int connect();
    void disconnect();
    bool isConnected();
    int startListening(int t_ms);
    void stopListening();
    void addListener(NodeKey type, evtHandler handler);

    // read values
    int readValueBool(UA_NodeId nodeid, UA_Variant &value);
    int readValueInt16(UA_NodeId nodeid, UA_Variant &value);
    int readValueInt32(UA_NodeId nodeid, UA_Variant &value);
    int readValueInt64(UA_NodeId nodeid, UA_Variant &value);
    int readValueUInt16(UA_NodeId nodeid, UA_Variant &value);
    int readValueUInt32(UA_NodeId nodeid, UA_Variant &value);
    int readValueUInt64(UA_NodeId nodeid, UA_Variant &value);
    // write values
    int writeValue(UA_NodeId nodeid, UA_Variant &newValue);
    int writeValue(UA_NodeId nodeid, int16_t value);
    int writeValue(UA_NodeId nodeid, int32_t value);
    int writeValue(UA_NodeId nodeid, uint16_t value);
    int writeValue(UA_NodeId nodeid, uint32_t value);
    int writeValue(UA_NodeId nodeid, bool value);
    // write arrays
    int writeValue(UA_NodeId nodeid, int16_t *value, int len);
    int writeValue(UA_NodeId nodeid, int32_t *value, int len);
    int writeValue(UA_NodeId nodeid, int64_t *value, int len);
    int writeValue(UA_NodeId nodeid, uint16_t *value, int len);
    int writeValue(UA_NodeId nodeid, uint32_t *value, int len);
    int writeValue(UA_NodeId nodeid, uint64_t *value, int len);
    int writeValue(UA_NodeId nodeid, bool *value, int len);

protected:
    void notify(opc_evt evt);
    bool checkFlag(UA_NodeId node);
    void clearFlag(UA_NodeId node);
    void clearAllFlags();

private:
    UA_Client* client;
    std::string endpoint;
    UA_StatusCode connectionStatus;
    volatile bool isListening = false;
    std::mutex opc_call_mtx;
    std::unordered_map<NodeKey, std::vector<evtHandler>, NodeKey::KeyHasher> listeners;
    std::unordered_set<NodeKey, NodeKey::KeyHasher> event_nodes;
};

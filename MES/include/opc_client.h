#pragma once

#include "l_comms.h"
#include <unordered_map>

#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>

enum opc_evt_type {
    REQ_ORDER, ORDER_BEGIN, ORDER_END, _END_EVT_
};

// template <typename T>
struct opc_evt
{
    opc_evt_type type;
    int data; // temporary testing
    // void *data;
    // std::size_t data_len;
};

class OpcClient
{
public:
    using evtHandler = std::function<void(opc_evt)>;

    OpcClient();
    ~OpcClient();
    int connect(const std::string &endpoint);
    void startListening(int t_ms);
    void stopListening();
    void addListener(opc_evt_type type, evtHandler handler);
    // void removeListener(opc_evt_type type, evtHandler handler);
private:
    void notify(opc_evt evt);

private:
    UA_Client* client;
    UA_StatusCode connectionStatus;
    volatile bool isListening;
    std::unordered_map<opc_evt_type, std::vector<evtHandler>, std::hash<int>> listeners;
};

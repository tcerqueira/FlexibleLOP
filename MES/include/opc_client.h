#pragma once

#include "l_comms.h"
#include <unordered_map>

#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>

enum opc_evt_type {
    REQ_ORDER, ORDER_BEGIN, ORDER_END, _END_EVT_
};

typedef struct
{
    opc_evt_type type;
    int data; // temporary testing
} opc_evt;

class OpcClient
{
public:
    typedef std::function<void(opc_evt)> evtHandler;

    OpcClient();
    ~OpcClient();
    int connect(const std::string &endpoint);
    void startListening(int t_ms);
    void stopListening();
    void addListener(opc_evt_type type, evtHandler handler);
    // void removeListener(opc_evt_type type, evtHandler handler);
private:
    // bool 
private:
    UA_Client* client;
    UA_StatusCode connectionStatus;
    volatile bool isListening;
    std::unordered_map<opc_evt_type, std::vector<evtHandler>, std::hash<int>> listeners;
};

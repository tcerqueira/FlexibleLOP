#pragma once

#include "l_comms.h"
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>

class OpcClient
{
public:
    OpcClient();
    ~OpcClient();
    int connect(const std::string &endpoint);
private:
    UA_Client* client;
    UA_StatusCode connectionStatus;
};

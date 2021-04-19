#pragma once

#include "l_comms.h"
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>

class opc_client
{
public:
    opc_client();
    ~opc_client();
    int connect(const char* endpoint);
private:
    UA_Client* client;
    UA_StatusCode connectionStatus;
};

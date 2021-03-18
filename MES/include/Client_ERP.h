#pragma once
#include "Dispatcher_ERP.h"

class Client_ERP
{
public:
    Client_ERP();
    Client_ERP(Dispatcher* dispatcher);
    void listen_async(const int& port);
private:
    Dispatcher *dispatcher;
};

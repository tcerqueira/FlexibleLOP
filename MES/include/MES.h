#pragma once

#include <iostream>
#include "Scheduler.h"
#include "Client_ERP.h"
#include "Dispatcher_ERP.h"
#include "Storage.h"
#include "LOProduction.h"

class MES
{
public:
    MES();
    void start();

private:
    int setUp();
    void run();

private:
    Scheduler scheduler;
    Dispatcher dispatcher;
    Storage store;
    LOProduction factory;
    Client_ERP erp;

    void onOrder(const char* bytes);
};

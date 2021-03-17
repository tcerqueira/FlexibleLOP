#pragma once

#include <iostream>
#include "Scheduler.h"
#include "RequestDispatcher.h"
#include "Storage.h"
#include "LOProduction.h"

class MES
{
public:
    MES();
    void run();

private:
    Scheduler scheduler;
    Dispatcher dispatcher;
    Storage store;
    LOProduction factory;

    void onOrder(const char* bytes);
};

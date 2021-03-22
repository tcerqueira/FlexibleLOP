#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include "Scheduler.h"
#include "udp_server.h"
#include "Dispatcher_ERP.h"
#include "Storage.h"
#include "LOProduction.h"

#define LISTEN_PORT 54321

class MES
{
public:
    MES();
    ~MES();
    void start();

private:
    int setUp();
    void run();

private:
    Scheduler scheduler;
    Dispatcher dispatcher;
    Storage store;
    LOProduction factory;
    UdpServer* erp_server;
    boost::asio::io_service io_service;

    void onOrder(const char* bytes);
};

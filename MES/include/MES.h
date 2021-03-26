#pragma once

#include "l_comms.h"
#include <boost/asio.hpp>
#include "Scheduler.h"
#include "udp_server.h"
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
    // request handlers
    void erpRequestDispatcher(char* data, std::size_t len, std::shared_ptr<std::string> response);
    void onOrder(const OrderNode& order_xml);
    void onStorageRequest();
    void onScheduleRequest();

private:
    Scheduler scheduler;
    Storage store;
    LOProduction factory;
    UdpServer* erp_server;
    boost::asio::io_service io_service;
};

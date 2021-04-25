#pragma once

#include "l_comms.h"
#include <boost/asio.hpp>
#include "Scheduler.h"
#include "udp_server.h"
#include "Storage.h"
#include "opc_client.h"
#include "XmlParser.h"

#define UDP_LISTEN_PORT 54321
#define OPC_LISTEN_PERIOD_MS 10000

class MES
{
public:
    MES();
    ~MES();
    void start();

private:
    void setUp();
    void run();
    // request handlers
    void erpRequestDispatcher(char* data, std::size_t len, std::shared_ptr<std::string> response);
    void onOrderRequest(const OrderNode& order_node, std::shared_ptr<std::string> response);
    void onStorageRequest(std::shared_ptr<std::string> response);
    void onScheduleRequest(std::shared_ptr<std::string> response);
    // Factory of orders from xml
    static Order *OrderFactory(const OrderNode &order_node);

private:
    Scheduler scheduler;
    Storage store;
    OpcClient* fct_client;
    UdpServer* erp_server;
    boost::asio::io_service io_service;
};

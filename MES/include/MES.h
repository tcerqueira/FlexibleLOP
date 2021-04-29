#pragma once

#include "l_comms.h"
#include <boost/asio.hpp>
#include "Scheduler.h"
#include "udp_server.h"
#include "Storage.h"
#include "opc_client.h"
#include "XmlParser.h"

class MES
{
public:
    MES();
    ~MES();
    void start();

private:
    void setUp();
    void run();
    // udp handlers
    void erpRequestDispatcher(char* data, std::size_t len, std::shared_ptr<std::string> response);
    void onOrderRequest(const OrderNode& order_node, std::shared_ptr<std::string> response);
    void onStorageRequest(std::shared_ptr<std::string> response);
    void onScheduleRequest(std::shared_ptr<std::string> response);
    // opc handlers
    // TODO here
    void onSendTransform();
    void onSendUnload();
    void onLoadPiece();
    void onStartOrder();
    void onFinishOrder();
    void onFinishProcessing();
    // Factory of orders from xml
    static Order *OrderFactory(const OrderNode &order_node);

private:
    boost::asio::io_service io_service;
    Scheduler scheduler;
    Storage store;
    OpcClient fct_client;
    UdpServer erp_server;
};

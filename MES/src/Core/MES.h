#pragma once

#include "l_comms.h"
#include <boost/asio.hpp>
#include "Scheduler.h"
#include "ERP/Udp_Server.h"
#include "Storage.h"
#include "Factory/Opc_Client.h"
#include "XmlParser/XmlParser.h"

#define OPC_GLOBAL_NODE_STR "|var|CODESYS Control Win V3 x64.Application.GVL."

class MES
{
public:
    MES(const std::string& opc_endpoint);
    MES(std::string&& opc_endpoint);
    void start();

private:
    void setUp();
    void run();
    void erpRequestDispatcher(char* data, std::size_t len, std::shared_ptr<std::string> response);
    // udp handlers
    void onOrderRequest(const OrderNode& order_node, std::shared_ptr<std::string> response);
    void onStorageRequest(std::shared_ptr<std::string> response);
    void onScheduleRequest(std::shared_ptr<std::string> response);
    // opc handlers
    void onSendTransform(int cell);
    void onSendUnload();
    void onLoadOrder();
    void onStartOrder();
    void onFinishOrder();
    void onUnloaded();
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
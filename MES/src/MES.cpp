#include <iostream>
#include <fstream>
#include <thread>
#include "MES.h"
#include "udp_server.h"
#include "Dispatcher_ERP.h"
#include "XmlParser.h"

MES::MES()
{
    scheduler = Scheduler();
    dispatcher = Dispatcher();
    erp_server = new UdpServer(io_service, LISTEN_PORT);
    store = Storage();
    factory = LOProduction();
}

void MES::run()
{
    std::thread erpServerThread([this]() {
        io_service.run();
    });

    char buf[50];
    while(1)
    {
        factory.listen();
        factory.send();
        std::cin >> buf;
    }

    erpServerThread.join();
}


void MES::start()
{
    std::cout << "Mes starting!" << std::endl;
    setUp();
    // TEST
    OrderDoc doc;
    doc.load_file("test/OrderTest.xml");
    // int index = 1;
    // std::cout << doc.number(index) << std::endl;
    // std::cout << doc.from(index) << std::endl;
    // std::cout << doc.to(index) << std::endl;
    // std::cout << doc.quantity(index) << std::endl;
    // std::cout << doc.time(index) << std::endl;
    // std::cout << doc.penalty(index) << std::endl;
    // std::cout << doc.maxdelay(index) << std::endl;
    OrderList* newList = OrderList::CreateOrders(doc);
    std::cout << newList->at(1)->getId() << std::endl;
    // TEST
    std::cout << "Mes running!" << std::endl;
    run();  
}

int MES::setUp()
{
    dispatcher.addOrderListener([](const Request_ERP& req, Response_ERP* res) { 
        std::cout << req.get() << std::endl;
    });
}

void MES::onOrder(const char* bytes)
{
    std::cout << bytes << std::endl;
}

MES::~MES()
{
    delete erp_server;
}

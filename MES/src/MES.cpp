#include <iostream>
#include <fstream>
#include "MES.h"
#include "Client_ERP.h"
#include "Dispatcher_ERP.h"
#include "XmlParser.h"

MES::MES()
{
    scheduler = Scheduler();
    dispatcher = Dispatcher();
    erp = Client_ERP(&dispatcher);
    store = Storage();
    factory = LOProduction();
}

void MES::run()
{
    char buf[50];
    while(1)
    {
        factory.listen();
        factory.send();
        std::cin >> buf;
    }
}


void MES::start()
{
    std::cout << "Mes starting!" << std::endl;
    setUp();
    // TEST
    OrderDoc doc;
    doc.load_file("test/OrderTest.xml");
    OrderList::CreateOrders(doc);
    // TEST
    std::cout << "Mes running!" << std::endl;
    run();    
}

int MES::setUp()
{
    dispatcher.addOrderListener([](const Request_ERP& req, Response_ERP* res) { 
        std::cout << req.get() << std::endl;
    });

    erp.listen_async(80);
}

void MES::onOrder(const char* bytes)
{
    std::cout << bytes << std::endl;
}
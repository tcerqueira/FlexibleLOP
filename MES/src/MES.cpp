#include <iostream>
#include <fstream>
#include "MES.h"
#include "Client_ERP.h"
#include "Dispatcher_ERP.h"

MES::MES()
{
    scheduler = Scheduler();
    dispatcher = Dispatcher();
    erp = Client_ERP(&dispatcher);
    store = Storage();
    factory = LOProduction();

    dispatcher.addOrderListener([](const Request_ERP& req, Response_ERP* res) { 
        std::cout << req.get() << std::endl;
    });
}

void MES::run()
{
    std::cout << "Mes running!" << std::endl;
    char buf[50];
    erp.listen_async(80);

    while(1)
    {
        factory.listen();
        factory.send();
        std::cin >> buf;
    }
}

void MES::onOrder(const char* bytes)
{
    std::cout << bytes << std::endl;
}
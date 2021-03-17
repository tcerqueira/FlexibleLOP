#include <iostream>
#include <fstream>
#include "MES.h"
#include "RequestDispatcher.h"

MES::MES()
{
    scheduler = Scheduler();
    dispatcher = Dispatcher();
    store = Storage();
    factory = LOProduction();
    dispatcher.addOrderListener([this](const char* bytes){
        onOrder(bytes);
    });
    dispatcher.addOrderListener([this](const char* bytes){
        std::cout << "Message received!" << std::endl;
    });
}

void MES::run()
{
    std::cout << "Mes running!" << std::endl;
    char buf[50];

    while(1)
    {
        factory.listen();
        factory.send();
        std::cin >> buf;
        dispatcher.dispatch(buf);
    }
}

void MES::onOrder(const char* bytes)
{
    std::cout << bytes << std::endl;
}
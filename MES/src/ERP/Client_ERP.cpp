#include "Client_ERP.h"
#include <thread>
#include "Dispatcher_ERP.h"

Client_ERP::Client_ERP()
{
}

Client_ERP::Client_ERP(Dispatcher* dispatcher)
{
    this->dispatcher = dispatcher;
}

void Client_ERP::listen_async(const int& port)
{
    std::thread listenerThread([this](const int& port) {
        while(1)
        {
            // listen
            // runs handler in other thread
            Response_ERP res;
            Request_ERP req = Request_ERP(
            "<Order Number=\"123\">"
                "<Unload Type=\"P1\" Destination=\"PM3\" Quantity=\"2\"/>"
            "</Order>"
            "<Order Number=\"321\">"
                "<Transform From=\"P3\" To=\"P4\" Quantity=\"5\" Time=\"1615744339\" MaxDelay=\"259200\""
                "Penalty=\"100\"/>"
            "</Order>");

            dispatcher->dispatch(req, &res);
            // TEST ASYNC
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }, port);

    listenerThread.detach();
}
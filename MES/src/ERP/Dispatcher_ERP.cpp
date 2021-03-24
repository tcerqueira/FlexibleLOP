#include "Dispatcher_ERP.h"

void Dispatcher::dispatch(const Request_ERP& req, Response_ERP* res)
{
    for(auto handler : requestHandlers)
        handler(req, res);
}

void Dispatcher::addListener(std::function<void(const Request_ERP&, Response_ERP*)> handler){
    requestHandlers.push_back(handler);
}

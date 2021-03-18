#include "Dispatcher_ERP.h"

void Dispatcher::dispatch(const Request_ERP& req, Response_ERP* res)
{
    for(auto handler : orderHandlers)
        handler(req, res);
    
    for(auto handler : storageHandlers)
        handler(req, res);
    
    for(auto handler : scheduleHandlers)
        handler(req, res);
}

void Dispatcher::addOrderListener(std::function<void(const Request_ERP&, Response_ERP*)> handler){
    orderHandlers.push_back(handler);
}

void Dispatcher::addStorageListener(std::function<void(const Request_ERP&, Response_ERP*)> handler)
{
    storageHandlers.push_back(handler);
}

void Dispatcher::addScheduleListener(std::function<void(const Request_ERP&, Response_ERP*)> handler)
{
    scheduleHandlers.push_back(handler);
}

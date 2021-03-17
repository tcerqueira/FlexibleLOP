#include "RequestDispatcher.h"
#include <functional>

void Dispatcher::dispatch(const char* bytes){
    for(auto handler : orderHandlers)
        handler(bytes);
    
    for(auto handler : storageHandlers)
        handler(bytes);
    
    for(auto handler : scheduleHandlers)
        handler(bytes);
}

void Dispatcher::addOrderListener(std::function<void(const char*)> handler){
    orderHandlers.push_back(handler);
}

void Dispatcher::addStorageListener(std::function<void(const char*)> handler)
{
    storageHandlers.push_back(handler);
}

void Dispatcher::addScheduleListener(std::function<void(const char*)> handler)
{
    scheduleHandlers.push_back(handler);
}

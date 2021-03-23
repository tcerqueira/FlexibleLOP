#pragma once

#include "l_comms.h"
#include "Request_Response_ERP.h"

class Dispatcher
{
public:
    typedef std::function<void(const Request_ERP&, Response_ERP*)> requestHandler;

    void dispatch(const Request_ERP& req, Response_ERP* res);
    void addOrderListener(requestHandler handler);
    void addStorageListener(requestHandler handler);
    void addScheduleListener(requestHandler handler);

private:
    // vector of handlers
    std::vector<requestHandler> orderHandlers;
    std::vector<requestHandler> storageHandlers;
    std::vector<requestHandler> scheduleHandlers;
};

#pragma once

#include "l_comms.h"

class Request_ERP;
class Response_ERP;

class Dispatcher
{
public:
    typedef std::function<void(const Request_ERP&, Response_ERP*)> requestHandler;

    void dispatch(const Request_ERP& req, Response_ERP* res);
    void addListener(requestHandler handler);

private:
    // vector of handlers
    std::vector<requestHandler> requestHandlers;
};

class Request_ERP
{
public:
    Request_ERP(const char* payload, std::size_t len);
    const char* get() const { return payload; }
private:
    const char* payload;
};

class Response_ERP
{
public:
    Response_ERP();
    Response_ERP(char* payload);
    char* get() { return payload; }
private:
    char* payload;
};

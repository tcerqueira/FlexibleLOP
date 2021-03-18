#pragma once

class Request_ERP
{
public:
    Request_ERP(const char* packet);
    const char* get() const { return packet; }
private:
    const char* packet;
};

class Response_ERP
{
public:
    Response_ERP();
    Response_ERP(char* packet);
    char* get() { return packet; }
private:
    char* packet;
};

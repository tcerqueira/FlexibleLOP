// Source@: https://www.boost.org/doc/libs/1_35_0/doc/html/boost_asio/tutorial/tutdaytime6.html

#pragma once

#include "l_comms.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#define MAX_BUFFER 65536

using boost::asio::ip::udp;

class UdpServer
{
public:
    typedef std::function<void(char*, std::size_t, std::shared_ptr<std::string>)> req_dispatcher;
    typedef std::function<void(std::shared_ptr<std::string>, std::size_t)> res_dispatcher;

    UdpServer(boost::asio::io_service& io_service, int port);
    void setRequestDispatcher(req_dispatcher dispatcher) { this->requestDispatcher = dispatcher; }
    void setResponseDispatcher(res_dispatcher dispatcher) { this->responseDispatcher = dispatcher; }

private:
    void start_receive();
    void handle_receive(const boost::system::error_code& error, std::size_t length);
    void handle_send(std::shared_ptr<std::string> message, const boost::system::error_code& error, std::size_t length);
private:
    req_dispatcher requestDispatcher = nullptr;
    res_dispatcher responseDispatcher = nullptr;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    char recv_buffer_[MAX_BUFFER];
    // std::string str_buffer;
};

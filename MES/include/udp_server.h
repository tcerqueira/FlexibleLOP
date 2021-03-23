// Source@: https://www.boost.org/doc/libs/1_35_0/doc/html/boost_asio/tutorial/tutdaytime6.html

#pragma once

#include "l_comms.h"
#include "Dispatcher_ERP.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#define MAX_BUFFER 4096

using boost::asio::ip::udp;

class UdpServer
{
public:
    UdpServer(Dispatcher* dispatcher);
    UdpServer(boost::asio::io_service& io_service, int port);

private:
    void start_receive();
    void handle_receive(const boost::system::error_code& error, std::size_t length);
    void handle_send(boost::shared_ptr<std::string> bytes, const boost::system::error_code& error, std::size_t length);
private:
    Dispatcher *dispatcher;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, MAX_BUFFER> recv_buffer_;
};

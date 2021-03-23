#include "udp_server.h"
#include <thread>
#include <iostream>
#include "Dispatcher_ERP.h"

UdpServer::UdpServer(boost::asio::io_service& io_service, int port)
: socket_(io_service, udp::endpoint(udp::v4(), port))
{
    start_receive();
}

void UdpServer::start_receive()
{
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&UdpServer::handle_receive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void UdpServer::handle_receive(const boost::system::error_code& error, std::size_t length)
{
    if (!error || error == boost::asio::error::message_size)
    {
        boost::shared_ptr<std::string> message(
            new std::string(std::to_string(length)));

        socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
            boost::bind(&UdpServer::handle_send, this, message,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

        start_receive();
    }
    std::cout << "Req received!" << std::endl;
}

void UdpServer::handle_send(boost::shared_ptr<std::string> bytes, const boost::system::error_code& error, std::size_t length)
{
    std::cout << "Res sent!" << std::endl;
}

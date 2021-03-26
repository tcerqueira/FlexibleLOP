#include "udp_server.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio.hpp>

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
    // auto db = boost::asio::dynamic_buffer(str_buffer);
    // auto buffer = db.prepare(MAX_BUFFER);

    // socket_.async_receive_from(
    //     buffer, remote_endpoint_,
    //     boost::bind(&UdpServer::handle_receive, this,
    //         boost::asio::placeholders::error,
    //         boost::asio::placeholders::bytes_transferred));
}

void UdpServer::handle_receive(const boost::system::error_code& error, std::size_t length)
{
    if (!error || error == boost::asio::error::message_size)
    {
        auto response = std::make_shared<std::string>();

        if(requestDispatcher != nullptr)
            requestDispatcher(recv_buffer_, length, response);

        socket_.async_send_to(boost::asio::buffer(*response), remote_endpoint_,
            boost::bind(&UdpServer::handle_send, this, response,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

        start_receive();
    }
    else{
        MES_ERROR("Error receiving request: {}", error.message());
    }
}

void UdpServer::handle_send(std::shared_ptr<std::string> message, const boost::system::error_code& error, std::size_t length)
{
    if(responseDispatcher != nullptr)
        responseDispatcher(message, length);
}

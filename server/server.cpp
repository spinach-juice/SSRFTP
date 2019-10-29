#include "server.h"

// constructor
// creates a server and a listener @ socket 13
server::server() : socket_(io_service, udp::endpoint(udp::v4(), 13))
{
    start_receive();
    io_service.run();
}

void server::start_receive()
{
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&server::handle_receive, this, 
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void server::handle_receive(const boost::system::error_code& error,
    std::size_t sz = MAX_SIZE)
{
     if (!error || error == boost::asio::error::message_size)
     {
        boost::shared_ptr<std::string> message(
          new std::string("This is a test bit string"));

        socket_.async_send_to(boost::asio::buffer(*message), 
            remote_endpoint_, boost::bind(&server::handle_send, 
            this, message,boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

        start_receive();
    }
}
 
void server::handle_send(boost::shared_ptr<std::string> /*message*/,
    const boost::system::error_code& /*error*/,
    std::size_t sz = MAX_SIZE/*bytes_transferred*/)
{

}

int main()
{}




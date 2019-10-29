#ifndef _SERVER_H
#define _SERVER_H


#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

#define MAX_SIZE 65536

class server
{
private:
    
    // used variables 
    boost::asio::io_service io_service;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, 1> recv_buffer_;
 
    
    void start_receive();
    void handle_receive(const boost::system::error_code& error,
        std::size_t);
    void handle_send(boost::shared_ptr<std::string> /*message*/,
        const boost::system::error_code& /*error*/,
        std::size_t);

public:
    server();
};

#endif

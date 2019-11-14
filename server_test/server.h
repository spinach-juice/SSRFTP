#ifndef _SERVER_H
#define _SERVER_H


#define MAX_SIZE 65536

#include <iostream>
#include <string>
#include <queue>
#include "communicator.h"
#include "packet.h"

class server
{
private:
    
    // ASIO communication variables
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    
    // Server Data Handling variables 
    std::queue<Message> send_buff;
    std::queue<Message> recv_buff;
    
    //logging file
    std::fstream logging;
    
//------ Methods ----------------------------------------
    
    void start_receive();
    
    // listens for packets and stores them temporarily in the recv_buff
    void receive_listener(); 
    
    // sends the packets from the send_buff
    void send_message();
    
public:
    // Constructors
    server();
    //server(boost::asio::io_service, udp::endpoint);
    
    void start_server();
};

#endif

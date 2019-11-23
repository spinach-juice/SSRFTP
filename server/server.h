#ifndef _SERVER_H
#define _SERVER_H


#define MAX_SIZE 65536

#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "packet.h"
#include "communicator.h"

using boost::asio::ip::udp;

typedef std::pair<Packet,std::string> Message;

Message package_message(const Packet& p, std::string endpoint);
Packet get_packet(const Message& m);
std::string get_endpoint(const Message& m);

class server
{
private:
   
    /*// ASIO communication variables
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint receive_endpoint;
    boost::asio::ip::udp::endpoint send_endpoint;*/
    
    std::queue<Message> recv_buff;
    std::queue<Message> send_buff;
    
    bool* shard_request_checker;
    
    Communicator* server_comm;
    
    std::fstream file;
    
    
public:   
    /*---------Constructor----------------------------*/
    server();
    ~server();
    
    void start_server();
    //void send();
    void kill();

};

#endif

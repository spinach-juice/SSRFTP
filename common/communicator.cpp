#include "communicator.h"
#include <pthread.h>
#include <stdexcept>
#include <boost/asio.hpp>
#include <iostream>
#include <unistd.h>

static void* shared_memory[3];

void* communicator_main_loop(void* args)
{
	bool* is_active = ((bool**)shared_memory)[0];
	std::queue<Message>* outgoing_msg = ((std::queue<Message>**)shared_memory)[1];
	std::queue<Message>* incoming_msg = ((std::queue<Message>**)shared_memory)[2];
	
	std::cout << "Initializing Communicator..." << std::endl;

	boost::asio::io_service comm_service;
	boost::asio::ip::udp::socket comm_socket(comm_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), UDP_PORT));

	std::cout << "Socket Opened. Listening..." << std::endl;

	while(*is_active)
	{
		if(!(outgoing_msg->empty()))
		{
			Message m = outgoing_msg->front();
			std::cout << "Sending message to " << get_endpoint(m) << std::endl;

			boost::asio::ip::udp::endpoint destination(boost::asio::ip::address::from_string(get_endpoint(m)), UDP_PORT);
			comm_socket.send_to(boost::asio::buffer(get_packet(m).bytestream(), get_packet(m).size()), destination);
			outgoing_msg->pop();
		}



		// Sleep is necessary - this is every 1ms
		usleep(1000);
	}

	std::cout << "Deinitializing Communicator..." << std::endl;

	*is_active = true;

	pthread_exit(nullptr);
}

Message package_message(const Packet& p, std::string endpoint)
{
	Message m(p, endpoint);

	return m;
}

Packet get_packet(const Message& m)
{
	return m.first;
}

std::string get_endpoint(const Message& m)
{
	return m.second;
}

Communicator::Communicator()
{
	this->start();
}

Communicator::~Communicator()
{
	this->kill();
}

void Communicator::start()
{
	if(!this->is_active)
	{
		this->is_active = true;

		shared_memory[0] = (void*)&this->is_active;
		shared_memory[1] = (void*)&this->outgoing_msg;
		shared_memory[2] = (void*)&this->incoming_msg;

		if(pthread_create(&this->thread, nullptr, &communicator_main_loop, nullptr))
		{
			this->is_active = false;
			throw std::runtime_error("Cannot create communicator thread");
		}
	}
}

void Communicator::kill()
{
	if(this->is_active)
	{
		this->is_active = false;
		while(!this->is_active);
		this->is_active = false;
	}
}

void Communicator::send_message(const Message m)
{
	this->outgoing_msg.push(m);
}

bool Communicator::message_available()
{
	return !(this->incoming_msg.empty());
}

Message Communicator::read_message()
{
	if(this->incoming_msg.empty())
		throw std::runtime_error("Communicator::read_message() cannot read message from empty message list");

	Message m = this->incoming_msg.front();
	this->incoming_msg.pop();
	return m;
}

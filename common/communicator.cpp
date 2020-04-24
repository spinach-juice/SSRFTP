#include "communicator.h"
#include <pthread.h>
#include <stdexcept>
#include <boost/asio.hpp>
#include <iostream>
#include <unistd.h>

// This global variable is necessary in order to pass objects between threads and calling function
static void* shared_memory[4];

// The main loop of the receiver thread.
void* reciever_main_loop(void* args)
{
	// Get values of shared memory.
	bool* is_active = ((bool**)shared_memory)[3];
	std::queue<Message>* incoming_msg = ((std::queue<Message>**)shared_memory)[2];

	// Set up socket.
	boost::asio::io_service comm_service;
	boost::asio::ip::udp::socket comm_socket(comm_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), UDP_RX_PORT));

	boost::asio::ip::udp::endpoint recieved_endpoint;
	unsigned char recv_buffer[65537];
	
	while(*is_active)
	{
		// This function blocks until something is received.
		comm_socket.receive_from(boost::asio::buffer(recv_buffer, 65537), recieved_endpoint);

		// Place recieved packet into the buffer.
		Packet p(recv_buffer);
		incoming_msg->push(package_message(p, recieved_endpoint.address().to_string()));
	}
	
	pthread_exit(nullptr);
}

// The main loop of the transceiver thread.
void* transciever_main_loop(void* args)
{
	// Get values from shared memory.
	bool* is_active = ((bool**)shared_memory)[0];
	std::queue<Message>* outgoing_msg = ((std::queue<Message>**)shared_memory)[1];

	// Set up socket.
	boost::asio::io_service comm_service;
	boost::asio::ip::udp::socket comm_socket(comm_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), UDP_TX_PORT));

	while(*is_active)
	{
		// Loop through the queue of messages to send.
		while(!(outgoing_msg->empty()))
		{
			// Send each message out.
			Message m = outgoing_msg->front();
			boost::asio::ip::udp::endpoint destination(boost::asio::ip::address::from_string(get_endpoint(m)), UDP_RX_PORT);
			comm_socket.send_to(boost::asio::buffer(get_packet(m).bytestream(), get_packet(m).size()), destination);
			outgoing_msg->pop();
		}

		// Sleep is necessary because there are no blocking calls here - this is every 100ms
		usleep(100000);
	}

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
	this->rx_active = false;
	this->tx_active = false;

	this->start();
}

Communicator::~Communicator()
{
	this->kill();
}

void Communicator::start()
{
	// If the transmitter thread is not active, activate it
	if(!this->tx_active)
	{
		this->tx_active = true;

		shared_memory[0] = (void*)&this->tx_active;
		shared_memory[1] = (void*)&this->outgoing_msg;

		if(pthread_create(&this->thread_trans, nullptr, &transciever_main_loop, nullptr))
		{
			this->tx_active = false;
			throw std::runtime_error("Cannot create transciever thread");
		}

	}

	// If the receiver thread is not active, activate it
	if(!this->rx_active)
	{
		this->rx_active = true;

		shared_memory[2] = (void*)&this->incoming_msg;
		shared_memory[3] = (void*)&this->rx_active;

		if(pthread_create(&this->thread_recv, nullptr, &reciever_main_loop, nullptr))
		{
			this->rx_active = false;
			throw std::runtime_error("Cannot create reciever thread");
		}
	}
}

void Communicator::kill()
{
	if(this->rx_active)
	{
		// This process is necessary because of the blocking receive call.
		this->rx_active = false;
		usleep(100000);
		pthread_cancel(this->thread_recv);
		pthread_join(this->thread_recv, nullptr);
	}

	if(this->tx_active)
	{
		this->tx_active = false;
		pthread_join(this->thread_trans, nullptr);
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

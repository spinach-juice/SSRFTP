#ifndef __COMMUNICATOR__H__
#define __COMMUNICATOR__H__

#define UDP_RX_PORT 13
#define UDP_TX_PORT 49197

#include "packet.h"
#include <string>
#include <queue>

typedef std::pair<Packet,std::string> Message;

Message package_message(const Packet& p, std::string endpoint);
Packet get_packet(const Message& m);
std::string get_endpoint(const Message& m);

class Communicator
{
public:

	Communicator();
	~Communicator();

	void start();
	void kill();
	void send_message(const Message);
	bool message_available();
	Message read_message();

private:

	pthread_t thread_trans;
	pthread_t thread_recv;
	std::queue<Message> outgoing_msg;
	std::queue<Message> incoming_msg;
	bool tx_active;
	bool rx_active;
};

#endif

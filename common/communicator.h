#ifndef __COMMUNICATOR__H__
#define __COMMUNICATOR__H__

#define UDP_PORT 13

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

	pthread_t thread;
	std::queue<Message> outgoing_msg;
	std::queue<Message> incoming_msg;
	bool is_active;
};

#endif

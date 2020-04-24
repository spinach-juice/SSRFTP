#ifndef __COMMUNICATOR__H__
#define __COMMUNICATOR__H__

// UDP communication port defines
#define UDP_RX_PORT 13
#define UDP_TX_PORT 49197

#include "packet.h"
#include <string>
#include <queue>

typedef std::pair<Packet,std::string> Message;

// Helper functions to get Packets and IP addresses out of Messages
Message package_message(const Packet& p, std::string endpoint);
Packet get_packet(const Message& m);
std::string get_endpoint(const Message& m);

// Class owns 2 threads, one for sending and one for receiving
class Communicator
{
public:

	Communicator();
	~Communicator();

	// Start is called automatically on instantiation
	// Can use start/kill to manage whether the communicator is active
	void start();
	void kill();

	// Places the message passes into the function into the queue to get sent out
	void send_message(const Message);

	// Checks whether a message is available to be read
	// MUST BE CHECKED BEFORE READING
	bool message_available();

	// Returns the next message that was received.
	Message read_message();

private:

	// Thread identifiers for sending and receiving threads
	pthread_t thread_trans;
	pthread_t thread_recv;

	// Data structures to store buffered messages
	std::queue<Message> outgoing_msg;
	std::queue<Message> incoming_msg;

	// Internal check variables to tell whether each component is active
	bool tx_active;
	bool rx_active;
};

#endif

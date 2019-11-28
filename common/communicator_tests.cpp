#include "catch.hpp"
#include "communicator.h"
#include "packet.h"
#include <boost/asio.hpp>

TEST_CASE("Communicator: Packet Reception")
{
	Communicator com;

	boost::asio::io_service io_service;
	boost::asio::ip::udp::socket skt(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 65535));

	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);

	usleep(100000);

	boost::asio::ip::udp::endpoint dest(boost::asio::ip::address::from_string("127.0.0.1"), UDP_RX_PORT);
	skt.send_to(boost::asio::buffer(p.bytestream(), p.size()), dest);

	usleep(100000);

	REQUIRE(com.message_available());
	Message m = com.read_message();
	Packet read_packet = get_packet(m);

	CHECK(p == read_packet);
	CHECK(get_endpoint(m) == "127.0.0.1");
}

TEST_CASE("Communicator: Packet Sending")
{
	Communicator com;

	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);

	usleep(100000);

	com.send_message(package_message(p, "127.0.0.1"));

	usleep(100000);

	REQUIRE(com.message_available());
	Message m = com.read_message();
	Packet read_packet = get_packet(m);

	CHECK(p == read_packet);
	CHECK(get_endpoint(m) == "127.0.0.1");
}

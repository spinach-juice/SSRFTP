#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "packet.h"
#include <cstring>

TEST_CASE("Packet Class: Bytestream constructor/accessor")
{
	char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);

	CHECK(strcmp(p.bytestream(), bytestream) == 0);
}

TEST_CASE("Packet Class: Copy constructor")
{
	char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);
	Packet p2(p);

	CHECK(strcmp(p2.bytestream(), bytestream) == 0);
}

TEST_CASE("Packet Class: Checksum Verifier")
{
	char valid_bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	char invalid_bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	valid_bytestream[2] = 0x00;
	valid_bytestream[3] = 0x31;
	invalid_bytestream[2] = 0x00;
	invalid_bytestream[3] = 0x31;
	valid_bytestream[4] = 0x11;
	valid_bytestream[5] = 0x1c;
	Packet p(valid_bytestream);
	Packet p2(invalid_bytestream);

	CHECK(p.verify_checksum());
	CHECK(!p2.verify_checksum());
}

TEST_CASE("Packet Class: Size accessor")
{
	char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);

	CHECK(p.size() == 0x32);
}

TEST_CASE("Packet Class: Packet Type accessor - type 0x0000")
{
	char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);

	CHECK(p.type() == (std::string)"Client Start Packet");
}

TEST_CASE("Packet Class: Packet Type accessor - type 0x0001")
{
	char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[1] = 0x01;
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);

	CHECK(p.type() == (std::string)"File Shard Packet");
}

TEST_CASE("Packet Class: Packet Type accessor - type 0x0002")
{
	char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[1] = 0x02;
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1e;
	Packet p(bytestream);

	CHECK(p.type() == (std::string)"Shard End Packet");
}

TEST_CASE("Packet Class: Packet Type accessor - type 0x0003")
{
	char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[1] = 0x03;
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1f;
	Packet p(bytestream);

	CHECK(p.type() == (std::string)"Shard Request Packet");
}

TEST_CASE("Packet Class: Packet Type accessor - type 0x0004")
{
	char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[1] = 0x04;
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x20;
	Packet p(bytestream);

	CHECK(p.type() == (std::string)"Transfer Complete Packet");
}

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "packet.h"
#include <cstring>

TEST_CASE("Packet Class: Bytestream constructor/accessor")
{
	char bytestream[] = "\0\0\02\0\0test string to represent a packet bytestream";
	bytestream[4] = 0x11;
	bytestream[5] = 0x1d;
	Packet p(bytestream);

	CHECK(strcmp(p.bytestream(), bytestream) == 0);
}

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "packet.h"
#include "util.h"

TEST_CASE("Packet Class: Bytestream constructor/accessor")
{
	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);

	CHECK(uchar_array_equal(p.bytestream(), bytestream, 32));
}

TEST_CASE("Packet Class: Copy constructor")
{
	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);
	Packet p2(p);

	CHECK(uchar_array_equal(p2.bytestream(), bytestream, 32));
}

TEST_CASE("Packet Class: Checksum Verifier")
{
	unsigned char valid_bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	unsigned char invalid_bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
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
	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);

	CHECK(p.size() == 0x32);
}

TEST_CASE("Packet Class: Packet Type accessor - type 0x0000")
{
	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x1c;
	Packet p(bytestream);

	CHECK(p.type() == (std::string)"Client Start Packet");
}

TEST_CASE("Packet Class: Packet Type accessor - type 0x0001")
{
	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
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
	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
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
	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
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
	unsigned char bytestream[] = "\0\0\0\0\0\0test string to represent a packet bytestream";
	bytestream[1] = 0x04;
	bytestream[2] = 0x00;
	bytestream[3] = 0x31;
	bytestream[4] = 0x11;
	bytestream[5] = 0x20;
	Packet p(bytestream);

	CHECK(p.type() == (std::string)"Transfer Complete Packet");
}

TEST_CASE("Packet Builders: Client Start")
{
	char md5[32] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	unsigned long long filesize = 0xab12cd34ef;
	unsigned long num_shards = 0xab2adf;
	unsigned short trans_id = 0x5a5a;
	char dest_path[] = "/home/output/file/path/file.txt";
	unsigned short path_length = 31;

	unsigned char valid_output[] = {0x00, 0x00, 0x00, 0x3f, 0x18, 0x94, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xab, 0x12, 0xcd, 0x34, 0xef, 0x00, 0xab, 0x2a, 0xdf, 0x5a, 0x5a, '/', 'h', 'o', 'm', 'e', '/', 'o', 'u', 't', 'p', 'u', 't', '/', 'f', 'i', 'l', 'e', '/', 'p', 'a', 't', 'h', '/', 'f', 'i', 'l', 'e', '.', 't', 'x', 't'};

	Packet p = build_client_start(md5, filesize, num_shards, trans_id, dest_path, path_length);

	CHECK(uchar_array_equal(p.bytestream(), valid_output, 64));
}

TEST_CASE("Packet Builders: File Shard")
{
	unsigned long shardnum = 0x617323;
	unsigned short trans_id = 0x5a5a;
	unsigned char filedata[1024];
	filedata[0] = 0;
	unsigned int i = 1;
	for(; i < 1024; i++)
		filedata[i] = filedata[i - 1] + 1;

	unsigned char valid_output[1036] = {0x00, 0x01, 0x04, 0x0b, 0xff, 0xbb, 0x00, 0x61, 0x73, 0x23, 0x5a, 0x5a};
	for(i = 0; i < 1024; i++)
		valid_output[i + 12] = filedata[i];

	Packet p = build_file_shard(shardnum, trans_id, filedata, 1024);

	CHECK(uchar_array_equal(p.bytestream(), valid_output, 1036));
}

TEST_CASE("Packet Builders: Shard End")
{
	unsigned short trans_id = 0x5a5a;
	unsigned char valid_output[8] = {0x00, 0x02, 0x00, 0x07, 0x00, 0xbd, 0x5a, 0x5a};

	Packet p = build_shard_end(trans_id);

	CHECK(uchar_array_equal(p.bytestream(), valid_output, 8));
}

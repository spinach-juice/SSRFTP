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

TEST_CASE("Packet Builders: Shard Request")
{
	unsigned long missing_nums[100] = {0x0000, 0x0003, 0x4158, 0x1122, 0x5673, 0xaaaa, 0xf437, 0x0005, 0x0010, 0x0020};

	int i = 10;
	for(; i < 100; i++)
		missing_nums[i] = missing_nums[i - 1] + 1;

	unsigned long num_miss = 100;

	unsigned short trans_id = 0x5a5a;
	unsigned char valid_output[] = {0x00, 0x03, 0x00, 0x37, 0x05, 0xbe, 0x5a, 0x5a, 0x00, 0x09, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x11, 0x22, 0x00, 0x00, 0x41, 0x58, 0x00, 0x00, 0x56, 0x73, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0xf4, 0x37, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x7a};

	Packet p = build_shard_request(trans_id, missing_nums, num_miss);

	CHECK(uchar_array_equal(p.bytestream(), valid_output, 56));
}

TEST_CASE("Packet Builders: Transfer Complete")
{
	unsigned short trans_id = 0x5a5a;
	bool success = true;

	unsigned char valid_output[] = {0x00, 0x04, 0x00, 0x08, 0x01, 0x40, 0x5a, 0x5a, 0x80};

	Packet p = build_transfer_complete(trans_id, success);

	CHECK(uchar_array_equal(p.bytestream(), valid_output, 9));
}

TEST_CASE("Packet Interpreters: Client Start")
{
	char md5[32] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	unsigned long long filesize = 0xab12cd34ef;
	unsigned long num_shards = 0xab2adf;
	unsigned short trans_id = 0x5a5a;
	char dest_path[] = "/home/output/file/path/file.txt";
	unsigned short path_length = 31;

	Packet p = build_client_start(md5, filesize, num_shards, trans_id, dest_path, path_length);

	char md5_out[32];
	unsigned long long filesize_out;
	unsigned long num_shards_out;
	unsigned short trans_id_out;
	char dest_path_out[260];
	unsigned short path_length_out;

	CHECK(interpret_client_start(p, md5_out, filesize_out, num_shards_out, trans_id_out, dest_path_out, path_length_out));
	CHECK(schar_array_equal(md5, md5_out, 32));
	CHECK(filesize_out == filesize);
	CHECK(num_shards_out == num_shards);
	CHECK(trans_id_out == trans_id);
	CHECK(schar_array_equal(dest_path, dest_path_out, 31));
	CHECK(path_length_out == path_length);
}

TEST_CASE("Packet Interpreters: File Shard")
{
	unsigned long shardnum = 0x617323;
	unsigned short trans_id = 0x5a5a;
	unsigned char filedata[1024];
	filedata[0] = 0;
	unsigned int i = 1;
	for(; i < 1024; i++)
		filedata[i] = filedata[i - 1] + 1;

	Packet p = build_file_shard(shardnum, trans_id, filedata, 1024);

	unsigned long shard_out;
	unsigned short trans_id_out;
	unsigned char filedata_out[1024];
	unsigned short size_out;

	CHECK(interpret_file_shard(p, shard_out, trans_id_out, filedata_out, size_out));
	CHECK(shardnum == shard_out);
	CHECK(trans_id == trans_id_out);
	CHECK(uchar_array_equal(filedata, filedata_out, 1024));
	CHECK(size_out == 1024);
}

TEST_CASE("Packet Interpreters: Shard End")
{
	unsigned short trans_id = 0x5a5a;

	Packet p = build_shard_end(trans_id);

	unsigned short trans_id_out;

	CHECK(interpret_shard_end(p, trans_id_out));
	CHECK(trans_id == trans_id_out);
}

TEST_CASE("Packet Interpreters: Shard Request")
{
	unsigned long missing_nums[100] = {0x0000, 0x0003, 0x4158, 0x1122, 0x5673, 0xaaaa, 0xf437, 0x0005, 0x0010, 0x0020};

	int i = 10;
	for(; i < 100; i++)
		missing_nums[i] = missing_nums[i - 1] + 1;

	unsigned long num_miss = 100;
	unsigned short trans_id = 0x5a5a;

	Packet p = build_shard_request(trans_id, missing_nums, num_miss);

	unsigned short trans_id_out;
	unsigned long missing_nums_out[100];
	unsigned long num_miss_out;

	ulong_array_sort(missing_nums, 100);

	CHECK(interpret_shard_request(p, trans_id_out, missing_nums_out, num_miss_out));
	CHECK(trans_id == trans_id_out);
	CHECK(ulong_array_equal(missing_nums, missing_nums_out, num_miss));
	CHECK(num_miss == num_miss_out);
}

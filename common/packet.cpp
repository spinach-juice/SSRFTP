#include "packet.h"

Packet::Packet(char const * const bytestream)
{
	this->data_size = ((unsigned short)bytestream[4] << 8) + (unsigned short)bytestream[5];

	this->data = new char[(int)(this->data_size) + 1];

	int i = 0;
	for(; i - 1 < this->data_size; i++)
		this->data[i] = bytestream[i];
}

Packet::Packet(const Packet&)
{

}

Packet::~Packet()
{
	delete this->data;
}

char const * const Packet::bytestream()
{
	return data;
}

bool Packet::verify_checksum()
{
	return false;
}

std::string Packet::type()
{
	return (std::string)"placeholder";
}

unsigned short Packet::size()
{
	return 0;
}

unsigned short Packet::calc_checksum()
{
	return 0;
}

Packet build_client_start(char* md5_chksum, unsigned long long file_size, unsigned long num_shards, unsigned short trans_id, char* destination_path, unsigned short path_length)
{
	Packet p("");
	return p;
}

Packet build_file_shard(unsigned long shard_num, unsigned short trans_id, char* shard_data, unsigned short data_size)
{
	Packet p("");
	return p;
}

Packet build_shard_end(unsigned short trans_id)
{
	Packet p("");
	return p;
}

Packet build_shard_request(unsigned short trans_id, unsigned long* missing_shards, unsigned long num_missing_shards)
{
	Packet p("");
	return p;
}

Packet build_transfer_complete(unsigned short trans_id, bool success_state)
{
	Packet p("");
	return p;
}

bool interpret_client_start(Packet& p, char*& md5_chksum, unsigned long long& file_size, unsigned long& num_shards, unsigned short& trans_id, char*& destination_path, unsigned short& path_length)
{
	return false;
}

bool interpret_file_shard(Packet& p, unsigned long& shard_num, unsigned short& trans_id, char*& shard_data, unsigned short& data_size)
{
	return false;
}

bool interpret_shard_end(Packet& p, unsigned short& trans_id)
{
	return false;
}

bool interpret_shard_request(Packet& p, unsigned short& trans_id, unsigned long*& missing_shards, unsigned long& num_missing_shards)
{
	return false;
}

bool interpret_transfer_complete(Packet& p, unsigned short& trans_id, bool& success_state)
{
	return false;
}

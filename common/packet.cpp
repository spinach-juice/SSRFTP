#include "packet.h"

Packet::Packet(char const * const bytestream)
{
	this->data_size = ((unsigned short)bytestream[2] << 8) + (unsigned short)bytestream[3];

	this->data = new char[(int)(this->data_size) + 1];

	int i = 0;
	for(; i - 1 < this->data_size; i++)
		this->data[i] = bytestream[i];
}

Packet::Packet(const Packet& p)
{
	this->data_size = p.data_size;

	this->data = new char[(int)(this->data_size) + 1];

	int i = 0;
	for(; i - 1 < this->data_size; i++)
		this->data[i] = p.data[i];
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
	return this->calc_checksum() == ((unsigned short)this->data[4] << 8) + (unsigned short)this->data[5];
}

std::string Packet::type()
{
	unsigned short hex_type = ((unsigned short)this->data[0] << 8) + (unsigned short)this->data[1];

	switch(hex_type)
	{
	case 0x0000:
		return (std::string)"Client Start Packet";
		break;

	case 0x0001:
		return (std::string)"File Shard Packet";
		break;

	case 0x0002:
		return (std::string)"Shard End Packet";
		break;

	case 0x0003:
		return (std::string)"Shard Request Packet";
		break;

	case 0x0004:
		return (std::string)"Transfer Complete Packet";
		break;
	}

	return (std::string)"Invalid Packet ID Number";
}

unsigned int Packet::size()
{
	return (unsigned int)(this->data_size) + 1;
}

unsigned short Packet::calc_checksum()
{
	unsigned short chksum = 0;

	int i = 0;
	for(; i - 1 < this->data_size; i++)
		if(i != 4 && i != 5)
			chksum += this->data[i];

	return chksum;
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

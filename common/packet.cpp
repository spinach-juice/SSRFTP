#include "packet.h"
#include "util.h"

Packet::Packet(unsigned char const * const bytestream)
{
	this->data_size = ((unsigned short)bytestream[2] << 8) + (unsigned short)bytestream[3];

	this->data = new unsigned char[(int)(this->data_size) + 1];

	int i = 0;
	for(; i - 1 < this->data_size; i++)
		this->data[i] = bytestream[i];
}

Packet::Packet(const Packet& p)
{
	this->data_size = p.data_size;

	this->data = new unsigned char[(int)(this->data_size) + 1];

	int i = 0;
	for(; i - 1 < this->data_size; i++)
		this->data[i] = p.data[i];
}

Packet::~Packet()
{
	delete this->data;
}

unsigned char const * const Packet::bytestream()
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

void Packet::replace_checksum()
{
	unsigned short chksum = this->calc_checksum();

	this->data[4] = (unsigned char)(chksum >> 8);
	this->data[5] = (unsigned char)(chksum & 0x00ff);
}

Packet build_client_start(char const * const md5_chksum, unsigned long long const file_size, unsigned long const num_shards, unsigned short const trans_id, char const * const destination_path, unsigned short const path_length)
{
	unsigned short packet_length = path_length + 32;
	unsigned char* bytes = new unsigned char[packet_length];
	bytes[0] = bytes[1] = 0x00;
	bytes[2] = (unsigned char)(packet_length >> 8);
	bytes[3] = (unsigned char)(packet_length & 0x00ff);
	ascii2hex(md5_chksum, &bytes[6], 32);
	bytes[22] = (unsigned char)((file_size & 0x000000ff00000000) >> 32);
	bytes[23] = (unsigned char)((file_size & 0x00000000ff000000) >> 24);
	bytes[24] = (unsigned char)((file_size & 0x0000000000ff0000) >> 16);
	bytes[25] = (unsigned char)((file_size & 0x000000000000ff00) >> 8);
	bytes[26] = (unsigned char)(file_size & 0x00000000000000ff);
	bytes[27] = (unsigned char)((num_shards & 0xff000000) >> 24);
	bytes[28] = (unsigned char)((num_shards & 0x00ff0000) >> 16);
	bytes[29] = (unsigned char)((num_shards & 0x0000ff00) >> 8);
	bytes[30] = (unsigned char)(num_shards & 0x000000ff);
	bytes[31] = (unsigned char)((trans_id & 0xff00) >> 8);
	bytes[32] = (unsigned char)(trans_id & 0x00ff);

	unsigned short i = 0;
	for(; i < path_length; i++)
		bytes[33 + i] = (unsigned char)destination_path[i];

	Packet p(bytes);
	p.replace_checksum();

	delete bytes;
	return p;
}

Packet build_file_shard(unsigned long const shard_num, unsigned short const trans_id, char const * const shard_data, unsigned short const data_size)
{
	unsigned char placeholder[12] = {'0'};
	Packet p(placeholder);
	return p;
}

Packet build_shard_end(unsigned short const trans_id)
{
	unsigned char placeholder[12] = {'0'};
	Packet p(placeholder);
	return p;
}

Packet build_shard_request(unsigned short const trans_id, unsigned long const * const missing_shards, unsigned long const num_missing_shards)
{
	unsigned char placeholder[12] = {'0'};
	Packet p(placeholder);
	return p;
}

Packet build_transfer_complete(unsigned short const trans_id, bool const success_state)
{
	unsigned char placeholder[12] = {'0'};
	Packet p(placeholder);
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

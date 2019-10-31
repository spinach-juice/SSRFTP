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

Packet build_file_shard(unsigned long const shard_num, unsigned short const trans_id, unsigned char const * const shard_data, unsigned short const data_size)
{
	unsigned short packet_length = data_size + 11;
	unsigned char* bytes = new unsigned char[packet_length];
	bytes[0] = 0x00;
	bytes[1] = 0x01;
	bytes[2] = (unsigned char)(packet_length >> 8);
	bytes[3] = (unsigned char)(packet_length & 0x00ff);
	bytes[6] = (unsigned char)((shard_num & 0xff000000) >> 24);
	bytes[7] = (unsigned char)((shard_num & 0x00ff0000) >> 16);
	bytes[8] = (unsigned char)((shard_num & 0x0000ff00) >> 8);
	bytes[9] = (unsigned char)(shard_num & 0x000000ff);
	bytes[10] = (unsigned char)((trans_id & 0xff00) >> 8);
	bytes[11] = (unsigned char)(trans_id & 0x00ff);

	unsigned int i = 0;
	for(; i < data_size; i++)
		bytes[12 + i] = shard_data[i];

	Packet p(bytes);
	p.replace_checksum();

	delete bytes;
	return p;
}

Packet build_shard_end(unsigned short const trans_id)
{
	unsigned char bytes[8] = {0x00, 0x02, 0x00, 0x07};
	bytes[6] = (unsigned char)((trans_id & 0xff00) >> 8);
	bytes[7] = (unsigned char)(trans_id & 0x00ff);

	Packet p(bytes);
	p.replace_checksum();

	return p;
}

Packet build_shard_request(unsigned short const trans_id, unsigned long const * const missing_shards, unsigned long const num_missing_shards)
{
	unsigned long * ordered_shards = new unsigned long[num_missing_shards];
	unsigned long i = 0;
	for(; i < num_missing_shards; i++)
		ordered_shards[i] = missing_shards[i];
	ulong_array_sort(ordered_shards, num_missing_shards);

	std::vector<unsigned long> singles = ulong_array_singles(ordered_shards, num_missing_shards);
	std::vector<unsigned long> ranges = ulong_array_ranges(ordered_shards, num_missing_shards);
	unsigned short packet_length = 11 + 4 * (singles.size() + ranges.size());

	unsigned char * bytes = new unsigned char[packet_length + 1];
	bytes[0] = 0x00;
	bytes[1] = 0x03;
	bytes[2] = (unsigned char)((packet_length & 0xff00) >> 8);
	bytes[3] = (unsigned char)(packet_length & 0x00ff);
	bytes[6] = (unsigned char)((trans_id & 0xff00) >> 8);
	bytes[7] = (unsigned char)(trans_id & 0x00ff);
	bytes[8] = (unsigned char)((singles.size() & 0xff00) >> 8);
	bytes[9] = (unsigned char)(singles.size() & 0x00ff);
	bytes[10] = (unsigned char)(((ranges.size() / 2) & 0xff00) >> 8);
	bytes[11] = (unsigned char)((ranges.size() / 2) & 0x00ff);
	for(i = 0; i < singles.size(); i++)
	{
		bytes[12 + (4 * i)] = (unsigned char)((singles.at(i) & 0xff000000) >> 24);
		bytes[13 + (4 * i)] = (unsigned char)((singles.at(i) & 0x00ff0000) >> 16);
		bytes[14 + (4 * i)] = (unsigned char)((singles.at(i) & 0x0000ff00) >> 8);
		bytes[15 + (4 * i)] = (unsigned char)(singles.at(i) & 0x000000ff);
	}
	unsigned short offset = 12 + (4 * i);
	for(i = 0; i < ranges.size(); i++)
	{
		bytes[offset + (4 * i)] = (unsigned char)((ranges.at(i) & 0xff000000) >> 24);
		bytes[offset + 1 + (4 * i)] = (unsigned char)((ranges.at(i) & 0x00ff0000) >> 16);
		bytes[offset + 2 + (4 * i)] = (unsigned char)((ranges.at(i) & 0x0000ff00) >> 8);
		bytes[offset + 3 + (4 * i)] = (unsigned char)(ranges.at(i) & 0x000000ff);
	}

	Packet p(bytes);
	p.replace_checksum();

	delete ordered_shards;
	delete bytes;
	return p;
}

Packet build_transfer_complete(unsigned short const trans_id, bool const success_state)
{
	unsigned char bytes[9] = {0x00, 0x04, 0x00, 0x08};
	bytes[6] = (unsigned char)((trans_id & 0xff00) >> 8);
	bytes[7] = (unsigned char)(trans_id & 0x00ff);
	if(success_state)
		bytes[8] = 0x80;
	else
		bytes[8] = 0x00;

	Packet p(bytes);
	p.replace_checksum();

	return p;
}

bool interpret_client_start(Packet& p, char* md5_chksum, unsigned long long& file_size, unsigned long& num_shards, unsigned short& trans_id, char* destination_path, unsigned short& path_length)
{
	if(p.bytestream()[0] != 0x00 || p.bytestream()[0] != 0x00 || p.size() < 32)
		return false;

	hex2ascii(&p.bytestream()[6], md5_chksum, 16);

	file_size = ((unsigned long long)(p.bytestream()[22]) << 32) | ((unsigned long long)(p.bytestream()[23]) << 24) | ((unsigned long long)(p.bytestream()[24]) << 16) | ((unsigned long long)(p.bytestream()[25]) << 8) | (unsigned long long)(p.bytestream()[26]);
	num_shards = ((unsigned long)(p.bytestream()[27]) << 24) | ((unsigned long)(p.bytestream()[28]) << 16) | ((unsigned long)(p.bytestream()[29]) << 8) | (unsigned long)(p.bytestream()[30]);
	trans_id = ((unsigned short)(p.bytestream()[31]) << 8) | (unsigned short)(p.bytestream()[32]);

	unsigned short i = 33;
	for(; i < p.size(); i++)
		destination_path[i - 33] = (char)(p.bytestream()[i]);
	path_length = i - 33;

	return p.verify_checksum();
}

bool interpret_file_shard(Packet& p, unsigned long& shard_num, unsigned short& trans_id, unsigned char* shard_data, unsigned short& data_size)
{
	if(p.bytestream()[0] != 0x00 || p.bytestream()[1] != 0x01 || p.size() < 12)
		return false;

	shard_num = (((unsigned long)(p.bytestream()[6])) << 24) | (((unsigned long)(p.bytestream()[7])) << 16) | (((unsigned long)(p.bytestream()[8])) << 8) | ((unsigned long)(p.bytestream()[9]));
	trans_id = (((unsigned short)(p.bytestream()[10])) << 8) | ((unsigned short)(p.bytestream()[11]));
	data_size = p.size() - 12;
	unsigned short i = 12;
	for(; i < p.size(); i++)
		shard_data[i - 12] = p.bytestream()[i];

	return p.verify_checksum();
}

bool interpret_shard_end(Packet& p, unsigned short& trans_id)
{
	if(p.bytestream()[0] != 0x00 || p.bytestream()[1] != 0x02 || p.size() != 8)
		return false;

	trans_id = (((unsigned short)(p.bytestream()[6])) << 8) | ((unsigned short)(p.bytestream()[7]));

	return p.verify_checksum();
}

bool interpret_shard_request(Packet& p, unsigned short& trans_id, unsigned long* missing_shards, unsigned long& num_missing_shards)
{
	if(p.bytestream()[0] != 0x00 || p.bytestream()[1] != 0x03 || p.size() < 10)
		return false;

	trans_id = (((unsigned short)(p.bytestream()[6])) << 8) | ((unsigned short)(p.bytestream()[7]));
	unsigned short num_singles = (((unsigned short)(p.bytestream()[8])) << 8) | ((unsigned short)(p.bytestream()[9]));
	unsigned short num_ranges = (((unsigned short)(p.bytestream()[10])) << 8) | ((unsigned short)(p.bytestream()[11]));

	unsigned long i = 12;
	num_missing_shards = 0;
	for(; num_missing_shards < num_singles; num_missing_shards++, i+=4)
	{
		missing_shards[num_missing_shards] = (((unsigned long)(p.bytestream()[i])) << 24) | (((unsigned long)(p.bytestream()[i+1])) << 16) | (((unsigned long)(p.bytestream()[i+2])) << 8) | ((unsigned long)(p.bytestream()[i+3]));
	}

	unsigned short j = 0;
	for(; j < num_ranges; j++, i+=8)
	{
		unsigned long range_start = (((unsigned long)(p.bytestream()[i])) << 24) | (((unsigned long)(p.bytestream()[i+1])) << 16) | (((unsigned long)(p.bytestream()[i+2])) << 8) | ((unsigned long)(p.bytestream()[i+3]));
		unsigned long range_end = (((unsigned long)(p.bytestream()[i+4])) << 24) | (((unsigned long)(p.bytestream()[i+5])) << 16) | (((unsigned long)(p.bytestream()[i+6])) << 8) | ((unsigned long)(p.bytestream()[i+7]));
		unsigned long k = range_start;
		for(; k <= range_end; k++, num_missing_shards++)
			missing_shards[num_missing_shards] = k;
	}

	ulong_array_sort(missing_shards, num_missing_shards);

	return p.verify_checksum();
}

bool interpret_transfer_complete(Packet& p, unsigned short& trans_id, bool& success_state)
{
	return p.verify_checksum();
}

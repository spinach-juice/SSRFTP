#ifndef __PACKET__H__
#define __PACKET__H__

#include <string>

class Packet
{
public:

	Packet(char const * const bytestream);
	Packet(const Packet&);
	~Packet();

	char const * const bytestream();
	bool verify_checksum();
	std::string type();
	unsigned int size();

private:

	unsigned short calc_checksum();

	char* data;
	unsigned short data_size;
};

Packet build_client_start(char* md5_chksum, unsigned long long file_size, unsigned long num_shards, unsigned short trans_id, char* destination_path, unsigned short path_length);
Packet build_file_shard(unsigned long shard_num, unsigned short trans_id, char* shard_data, unsigned short data_size);
Packet build_shard_end(unsigned short trans_id);
Packet build_shard_request(unsigned short trans_id, unsigned long* missing_shards, unsigned long num_missing_shards);
Packet build_transfer_complete(unsigned short trans_id, bool success_state);



bool interpret_client_start(Packet& p, char*& md5_chksum, unsigned long long& file_size, unsigned long& num_shards, unsigned short& trans_id, char*& destination_path, unsigned short& path_length);
bool interpret_file_shard(Packet& p, unsigned long& shard_num, unsigned short& trans_id, char*& shard_data, unsigned short& data_size);
bool interpret_shard_end(Packet& p, unsigned short& trans_id);
bool interpret_shard_request(Packet& p, unsigned short& trans_id, unsigned long*& missing_shards, unsigned long& num_missing_shards);
bool interpret_transfer_complete(Packet& p, unsigned short& trans_id, bool& success_state);

#endif

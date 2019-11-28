#ifndef __PACKET__H__
#define __PACKET__H__

#include <string>

class Packet
{
public:

	Packet(unsigned char const * const bytestream);
	Packet(const Packet&);
	~Packet();

	unsigned char const * const bytestream();
	bool verify_checksum();
	std::string type();
	unsigned short int_type();
	unsigned int size();
	void replace_checksum();

	friend bool operator==(Packet&, Packet&);
private:

	unsigned short calc_checksum();

	unsigned char* data;
	unsigned short data_size;

};

bool operator==(Packet& p1, Packet& p2);

Packet build_client_start(char const * const md5_chksum, unsigned long long const file_size, unsigned long const num_shards, unsigned short const trans_id, char const * const destination_path, unsigned short const path_length);
Packet build_file_shard(unsigned long const shard_num, unsigned short const trans_id, unsigned char const * const shard_data, unsigned short const data_size);
Packet build_shard_end(unsigned short const trans_id);
Packet build_shard_request(unsigned short const trans_id, unsigned long const * const missing_shards, unsigned long const num_missing_shards);
Packet build_transfer_complete(unsigned short const trans_id, bool const success_state);



bool interpret_client_start(Packet& p, char* md5_chksum, unsigned long long& file_size, unsigned long& num_shards, unsigned short& trans_id, char* destination_path, unsigned short& path_length);
bool interpret_file_shard(Packet& p, unsigned long& shard_num, unsigned short& trans_id, unsigned char* shard_data, unsigned short& data_size);
bool interpret_shard_end(Packet& p, unsigned short& trans_id);
bool interpret_shard_request(Packet& p, unsigned short& trans_id, unsigned long* missing_shards, unsigned long& num_missing_shards);
bool interpret_transfer_complete(Packet& p, unsigned short& trans_id, bool& success_state);

#endif

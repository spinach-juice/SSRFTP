#ifndef __PACKET__H__
#define __PACKET__H__

#include <string>
#include <vector>

// Packet class is basically just an augmented array.
// Automatically handles array size and the checksum field.
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

// Packet Builder functions. These aid in making valid Packets from "nice" variables.
// Packet definitions are per the Protocol specified in the User Manual.
Packet build_client_start(char const * const md5_chksum, unsigned long long const file_size, unsigned long const num_shards, unsigned short const trans_id, char const * const destination_path, unsigned short const path_length);
Packet build_file_shard(unsigned long const shard_num, unsigned short const trans_id, unsigned char const * const shard_data, unsigned short const data_size);
Packet build_shard_end(unsigned short const trans_id);
Packet build_shard_request(unsigned short const trans_id, unsigned long const * const missing_shards, unsigned long const num_missing_shards);
Packet build_transfer_complete(unsigned short const trans_id, bool const success_state);
Packet build_shard_request_range(unsigned short const trans_id, unsigned long const * const missing_shards, unsigned short const num_missing_shards, unsigned long const * const missing_ranges, unsigned short const num_missing_ranges);
Packet build_semi_robust_shard(unsigned short const trans_id, bool const final_shard, unsigned long const shard_num, unsigned char const * const shard_data, unsigned short const data_size);


// Packet Interpreter functions. These are just the exact reverse of the Packet Builder functions.
// The return value is true if the pakcet has a valid checksum and false otherwise.
// Variables other than the Packet passed in WILL BE MODIFIED to match the Packet contents.
bool interpret_client_start(Packet& p, char* md5_chksum, unsigned long long& file_size, unsigned long& num_shards, unsigned short& trans_id, char* destination_path, unsigned short& path_length);
bool interpret_file_shard(Packet& p, unsigned long& shard_num, unsigned short& trans_id, unsigned char* shard_data, unsigned short& data_size);
bool interpret_shard_end(Packet& p, unsigned short& trans_id);
bool interpret_shard_request(Packet& p, unsigned short& trans_id, unsigned long* missing_shards, unsigned long& num_missing_shards);
bool interpret_shard_request_range(Packet& p, unsigned short& trans_id, std::vector<unsigned long>& missing_singles, std::vector<unsigned long>& missing_ranges);
bool interpret_transfer_complete(Packet& p, unsigned short& trans_id, bool& success_state);
bool interpret_semi_robust_shard(Packet& p, unsigned short& trans_id, bool& final_shard, unsigned long& shard_num, unsigned char* shard_data, unsigned short& data_size);

#endif

#ifndef __SHARD__MANAGER__H__
#define __SHARD__MANAGER__H__

#include <vector>

#define SHARD_SIZE_MAX 65524

class ShardManager
{
public:
	ShardManager(char const * const filename, unsigned short trans_id); // constructor for file on local system
	ShardManager(char const * const filename, unsigned short trans_id, unsigned long const num_shards);
	ShardManager(const ShardManager&);
	~ShardManager();

	unsigned long num_shards();
	unsigned char* get_shard_data(unsigned long const shard_num, unsigned short& shard_size);
	void remove_shard(unsigned long const shard_num);
	void remove_shard_range(unsigned long const lower_bound, unsigned long const upper_bound);
	unsigned long* get_shard_singles(unsigned long& num_singles);
	unsigned long* get_shard_ranges(unsigned long& num_ranges);

	void add_shard(unsigned long const shard_num, unsigned char const * const shard_data, unsigned short const shard_size);
	bool shard_available(unsigned long const shard_num);

	bool is_done();

private:
	char attached_file[260];
	unsigned long shard_max;
	std::vector<unsigned long> shard_ranges;
	std::vector<unsigned long> shard_singles;
	unsigned char shard_data[SHARD_SIZE_MAX];
	unsigned short transfer_id;
	bool fill_mode;

	unsigned long* range_array;
	unsigned long* single_array;
};

#endif

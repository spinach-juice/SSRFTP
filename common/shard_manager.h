#ifndef __SHARD__MANAGER__H__
#define __SHARD__MANAGER__H__

class ShardManager
{
public:
	ShardManager(char const * const filename); // constructor for file on local system
	ShardManager(char const * const filename, unsigned long long const filesize, unsigned long const num_shards);
	ShardManager(const ShardManager&);
	~ShardManager();

	unsigned long num_shards();
	unsigned char* get_shard_data(unsigned long const shard_num, unsigned short& shard_size);
	void remove_shard(unsigned long const shard_num);
	void remove_shard_range(unsigned long const lower_bound, unsigned long const upper_bound);
	unsigned long* get_shard_singles(unsigned long& num_singles);
	unsigned long* get_shard_ranges(unsigned long& num_ranges);

	void add_shard(unsigned long const shard_num, unsigned char const * const shard_data, unsigned short const shard_size);
	unsigned long* get_missing_shard_singles(unsigned long& num_singles);
	unsigned long* get_missing_shard_ranges(unsigned long& num_ranges);

	bool is_done();
	void finalize();
private:

};

#endif

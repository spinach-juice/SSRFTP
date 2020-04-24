#ifndef __SHARD__MANAGER__H__
#define __SHARD__MANAGER__H__

#include <vector>
#include <string>
#include "range_list.h"

// This is the absolute maximum shard size based on UDP standard and packet definitions
#define SHARD_SIZE_MAX 65495

class ShardManager
{
public:
	// constructor for file on local system - good for client
	ShardManager(char const * const filename, unsigned short trans_id);
	// Constructor for file coming from remote - good for server
	ShardManager(char const * const filename, unsigned short trans_id, unsigned long const num_shards);
	// Copy constructor (avoid in normal use)
	ShardManager(const ShardManager&);
	// Base constructor (avoid in normal use)
	ShardManager();
	// Destructor
	~ShardManager();

	// Returns the number of shards in the file
	unsigned long num_shards();
	// Returns a point to an array of shard data, and puts the size of the array into shard_size
	unsigned char* get_shard_data(unsigned long const shard_num, unsigned short& shard_size);
	// Removes the shard from the list
	void remove_shard(unsigned long const shard_num);
	// Removes a range of shards from the list
	void remove_shard_range(unsigned long const lower_bound, unsigned long const upper_bound);
	// Gets a list of isolated shards left
	unsigned long* get_shard_singles(unsigned long& num_singles);
	// Gets a list of ranges of shards left
	unsigned long* get_shard_ranges(unsigned long& num_ranges);

	// Adds a shard to the file
	void add_shard(unsigned long const shard_num, unsigned char const * const shard_data, unsigned short const shard_size);
	// Tests whether a shard is available
	bool shard_available(unsigned long const shard_num);

	// Forces the range of a shard request onto the shard manager
	void impress_request(std::vector<unsigned long> singles, std::vector<unsigned long> ranges);

	// Returns the filename attached to this manager
	std::string get_filename();

	// Tests whether transfer is done
	bool is_done();

	// Permanently disable this shard manager
	void disable();

private:
	RangeList rl;

	char attached_file[260];
	unsigned long shard_max;
	std::vector<unsigned long> shard_ranges;
	std::vector<unsigned long> shard_singles;
	unsigned char shard_data[SHARD_SIZE_MAX];
	unsigned short transfer_id;
	bool fill_mode;

	unsigned long* range_array;
	unsigned long* single_array;

	bool disabled;
};

#endif

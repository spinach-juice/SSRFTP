#include "shard_manager.h"

ShardManager::ShardManager(char const * const filename)
{

}

ShardManager::ShardManager(char const * const filename, unsigned long long const filesize, unsigned long const num_shards)
{

}

ShardManager::ShardManager(const ShardManager&)
{

}

ShardManager::~ShardManager()
{

}

unsigned long ShardManager::num_shards()
{

}

unsigned char* ShardManager::get_shard_data(unsigned long const shard_num, unsigned short& shard_size)
{

}

void ShardManager::remove_shard(unsigned long const shard_num)
{

}

void ShardManager::remove_shard_range(unsigned long const lower_bound, unsigned long const upper_bound)
{

}

unsigned long* ShardManager::get_shard_singles(unsigned long& num_singles)
{

}

unsigned long* ShardManager::get_shard_ranges(unsigned long& num_ranges)
{

}

void ShardManager::add_shard(unsigned long const shard_num, unsigned char const * const shard_data, unsigned short const shard_size)
{

}

unsigned long* ShardManager::get_missing_shard_singles(unsigned long& num_singles)
{

}

unsigned long* ShardManager::get_missing_shard_ranges(unsigned long& num_ranges)
{

}

bool ShardManager::is_done()
{

}

void ShardManager::finalize()
{

}

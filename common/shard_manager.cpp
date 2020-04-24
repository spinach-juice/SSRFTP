#include "shard_manager.h"
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <iostream>

ShardManager::ShardManager(char const * const filename, unsigned short const trans_id)
{
	// File is coming from disk, so fill mode is off
	this->fill_mode = false;
	strcpy(this->attached_file, filename);

	FILE* open_file = nullptr;

	// Get file size
	open_file = fopen(filename, "r");
	if(open_file == nullptr)
		throw std::runtime_error("ShardManager could not open given file.");

	fseek(open_file, 0, SEEK_END);

	long file_size = ftell(open_file);

	// Number of shards is derived from file size
	this->shard_max = (unsigned long)file_size / SHARD_SIZE_MAX;
	if((unsigned long)file_size % SHARD_SIZE_MAX == 0)
		this->shard_max--;

	fclose(open_file);

	this->rl.init(0, this->shard_max);

	this->single_array = nullptr;
	this->range_array = nullptr;
	this->transfer_id = trans_id;
	this->disabled = false;
}

ShardManager::ShardManager(char const * const filename, unsigned short const trans_id, unsigned long const num_shards)
{
	// File is coming from remote, so fill mode is on
	this->fill_mode = true;
	strcpy(this->attached_file, filename);
	if(num_shards > 1)
	{
		this->shard_ranges.push_back(0);
		this->shard_ranges.push_back(num_shards - 1);
	}
	else
		this->shard_singles.push_back(0);

	this->rl.init(0, num_shards - 1);

	this->shard_max = num_shards - 1;
	this->single_array = nullptr;
	this->range_array = nullptr;
	this->transfer_id = trans_id;
	this->disabled = false;
}

ShardManager::ShardManager(const ShardManager& copy)
{
	this->fill_mode = copy.fill_mode;
	this->shard_max = copy.shard_max;
	this->rl = copy.rl;
	this->transfer_id = copy.transfer_id;
	strcpy(this->attached_file, copy.attached_file);
	this->disabled = copy.disabled;
}

ShardManager::ShardManager()
{
	this->disabled = true;
}

ShardManager::~ShardManager()
{
	if(this->range_array != nullptr)
		delete[] this->range_array;
	if(this->single_array != nullptr)
		delete[] this->single_array;

	if(!this->fill_mode || this->disabled)
		return;

	// Collate all shards into final file here
	FILE* output_file = fopen(this->attached_file, "w");
	if(output_file == nullptr)
	{
		std::cerr << "ShardManager could not write shards to output file\r\n";
		return;
	}

	for(unsigned long i = 0; i < this->shard_max + 1; i++)
	{
		char filename[40] = {0};
		sprintf(filename, "fs%utr%u.ssrftp", (unsigned int)i, (unsigned int)this->transfer_id);
		FILE* shard_file = fopen(filename, "r");
		if(shard_file == nullptr)
		{
			std::cerr << "ShardManager missing shard it thought it had\r\n";
			fclose(output_file);
			return;
		}

		unsigned short shard_size = (unsigned short)fread(this->shard_data, 1, SHARD_SIZE_MAX, shard_file);
		fclose(shard_file);

		unsigned short written_size = (unsigned short)fwrite(this->shard_data, 1, shard_size, output_file);
		if(shard_size != written_size)
		{
			std::cerr << "ShardManager could not write shards to output file\r\n";
			fclose(output_file);
			return;
		}

		// delete shard file
		remove(filename);
	}


	fclose(output_file);
}

unsigned long ShardManager::num_shards()
{
	if(this->disabled)
		return 0;
	return this->shard_max + 1;
}

unsigned char* ShardManager::get_shard_data(unsigned long const shard_num, unsigned short& shard_size)
{
	if(this->disabled)
		return nullptr;

	if(!this->shard_available(shard_num))
		throw std::runtime_error("Attempted to get data from a shard that does not exist in ShardManager");

	FILE* open_file = fopen(this->attached_file, "r");
	if(open_file == nullptr)
		throw std::runtime_error("ShardManager could not open given file.");

	fseek(open_file, shard_num * SHARD_SIZE_MAX, SEEK_SET);

	shard_size = (unsigned short)fread(this->shard_data, 1, SHARD_SIZE_MAX, open_file);
	return this->shard_data;
}

void ShardManager::remove_shard(unsigned long const shard_num)
{
	this->rl.remove(shard_num);
}

void ShardManager::remove_shard_range(unsigned long const lower_bound, unsigned long const upper_bound)
{
	for(unsigned long i = lower_bound; i <= upper_bound; i++)
		this->remove_shard(i);
}

unsigned long* ShardManager::get_shard_singles(unsigned long& num_singles)
{
	if(this->disabled)
		return nullptr;
	return this->rl.get_single_list(num_singles);
}

unsigned long* ShardManager::get_shard_ranges(unsigned long& num_ranges)
{
	if(this->disabled)
		return nullptr;

	unsigned long* array = this->rl.get_range_list(num_ranges);
	num_ranges /= 2;
	return array;
}

void ShardManager::add_shard(unsigned long const shard_num, unsigned char const * const shard_data, unsigned short const shard_size)
{
	if(this->disabled || this->shard_available(shard_num) || !this->fill_mode)
		return;

	this->rl.remove(shard_num);
	// write shard to a shard file
	
	char filename[40] = {0};
	sprintf(filename, "fs%utr%u.ssrftp", (unsigned int)shard_num, (unsigned int)this->transfer_id);

	FILE* open_file = fopen(filename, "w");
	if(open_file == nullptr)
		throw std::runtime_error("ShardManager could not write shard file");

	size_t written_num = fwrite(shard_data, 1, shard_size, open_file);

	if(written_num != (size_t)shard_size)
		throw std::runtime_error("ShardManager could not write shard file");

	fclose(open_file);
}

bool ShardManager::shard_available(unsigned long const shard_num)
{
	if(this->disabled)
		return false;

	if(this->fill_mode)
		return !this->rl.is_in_list(shard_num);
	return this->rl.is_in_list(shard_num);
}

void ShardManager::impress_request(std::vector<unsigned long> singles, std::vector<unsigned long> ranges)
{
	this->rl.impress(singles,ranges);
}

bool ShardManager::is_done()
{
	if(this->disabled)
		return false;

	unsigned long size1 = 0;
	unsigned long size2 = 0;
	this->rl.get_single_list(size1);
	this->rl.get_range_list(size2);

	return size1 == 0 && size2 == 0;
}

std::string ShardManager::get_filename()
{
	if(this->disabled)
		return "";
	return (std::string)this->attached_file;
}

void ShardManager::disable()
{
	this->disabled = true;
}

#include "shard_manager.h"
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <iostream>

ShardManager::ShardManager(char const * const filename, unsigned short const trans_id)
{
	this->fill_mode = false;
	strcpy(this->attached_file, filename);

	FILE* open_file = nullptr;

	open_file = fopen(filename, "r");
	if(open_file == nullptr)
		throw std::runtime_error("ShardManager could not open given file.");

	fseek(open_file, 0, SEEK_END);

	long file_size = ftell(open_file);

	this->shard_max = (unsigned long)file_size / SHARD_SIZE_MAX;
	if((unsigned long)file_size / SHARD_SIZE_MAX > 0)
		this->shard_max++;

	fclose(open_file);

	this->shard_ranges.push_back(0);
	this->shard_ranges.push_back(this->shard_max);
	this->return_array = nullptr;
	this->transfer_id = trans_id;
}

ShardManager::ShardManager(char const * const filename, unsigned short const trans_id, unsigned long const num_shards)
{
	this->fill_mode = true;
	strcpy(this->attached_file, filename);

	this->shard_ranges.push_back(0);
	this->shard_ranges.push_back(num_shards);

	this->shard_max = num_shards;
	this->return_array = nullptr;
	this->transfer_id = trans_id;
}

ShardManager::ShardManager(const ShardManager& copy)
{
	this->fill_mode = copy.fill_mode;
	this->shard_max = copy.shard_max;
	this->shard_singles = copy.shard_singles;
	this->shard_ranges = copy.shard_ranges;
	strcpy(this->attached_file, copy.attached_file);
}

ShardManager::~ShardManager()
{
	if(this->return_array != nullptr)
		delete[] this->return_array;

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
	}


	fclose(output_file);
}

unsigned long ShardManager::num_shards()
{
	return this->shard_max;
}

unsigned char* ShardManager::get_shard_data(unsigned long const shard_num, unsigned short& shard_size)
{
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
	if(!this->shard_available(shard_num) || this->fill_mode)
		return;

	for(unsigned long i = 0; i < this->shard_singles.size(); i++)
	{
		if(this->shard_singles.at(i) == shard_num)
		{
			this->shard_singles.erase(this->shard_singles.begin() + i);
			return;
		}
	}

	unsigned long i = 0;
	while(i < this->shard_ranges.size() && this->shard_ranges.at(i) < shard_num)
	{
		i++;

		if(this->shard_ranges.at(i) > shard_num)
		{
			unsigned long next_range_end = this->shard_ranges.at(i);
			this->shard_ranges.at(i) = shard_num - 1;
			this->shard_ranges.insert(this->shard_ranges.begin() + i + 1, next_range_end);
			this->shard_ranges.insert(this->shard_ranges.begin() + i + 1, shard_num + 1);
			if(this->shard_ranges.at(i) == this->shard_ranges.at(i - 1))
			{
				this->shard_singles.push_back(shard_num - 1);
				this->shard_ranges.erase(this->shard_ranges.begin() + i - 1, this->shard_ranges.begin() + i);
			}
			if(this->shard_ranges.at(i + 1) == this->shard_ranges.at(i + 2))
			{
				this->shard_singles.push_back(shard_num + 1);
				this->shard_ranges.erase(this->shard_ranges.begin() + i + 1, this->shard_ranges.begin() + i + 2);
			}
			return;
		}
		else if(this->shard_ranges.at(i) == shard_num)
		{
			this->shard_ranges.at(i)--;
			if(this->shard_ranges.at(i) == this->shard_ranges.at(i - 1))
			{
				this->shard_singles.push_back(shard_num - 1);
				this->shard_ranges.erase(this->shard_ranges.begin() + i - 1, this->shard_ranges.begin() + i);
			}
			return;
		}

		i++;
	}

	if(this->shard_ranges.at(i) == shard_num)
	{
		this->shard_ranges.at(i) = shard_num + 1;
		if(this->shard_ranges.at(i) == this->shard_ranges.at(i + 1))
		{
			this->shard_singles.push_back(shard_num + 1);
			this->shard_ranges.erase(this->shard_ranges.begin() + i, this->shard_ranges.begin() + i + 1);
		}
	}
}

void ShardManager::remove_shard_range(unsigned long const lower_bound, unsigned long const upper_bound)
{
	if(this->fill_mode)
		return;

	for(unsigned long i = 0; i < this->shard_singles.size(); i++)
		if(this->shard_singles.at(i) >= lower_bound && this->shard_singles.at(i) <= upper_bound)
			this->shard_singles.erase(this->shard_singles.begin() + i);

	unsigned long first_inside = 0;
	while(first_inside < this->shard_ranges.size() && this->shard_ranges.at(first_inside) < lower_bound)
		first_inside++;

	if(first_inside == this->shard_ranges.size())
		return;

	unsigned long last_inside = shard_ranges.size() - 1;
	while(last_inside != ((unsigned long)0) - 1 && this->shard_ranges.at(last_inside) > upper_bound)
		last_inside--;

	if(last_inside == ((unsigned long)0) - 1)
		return;

	if(first_inside & 1) // first range bound inside given range is an upper bound
	{
		if(last_inside & 1) // last range bound inside given range is an upper bound
		{
			if(first_inside == last_inside) // range contains only this upper bound
			{
				this->shard_ranges.at(first_inside) = lower_bound - 1;
				if(this->shard_ranges.at(first_inside) == this->shard_ranges.at(first_inside - 1))
				{
					this->shard_singles.push_back(this->shard_ranges.at(first_inside));
					this->shard_ranges.erase(this->shard_ranges.begin() + first_inside - 1, this->shard_ranges.begin() + first_inside);
				}
			}
			else // range spans multiple internal ranges
			{
				this->shard_ranges.erase(this->shard_ranges.begin() + first_inside + 1, this->shard_ranges.begin() + last_inside);
				this->shard_ranges.at(first_inside) = lower_bound - 1;
				if(this->shard_ranges.at(first_inside) == this->shard_ranges.at(first_inside - 1))
				{
					this->shard_singles.push_back(this->shard_ranges.at(first_inside));
					this->shard_ranges.erase(this->shard_ranges.begin() + first_inside - 1, this->shard_ranges.begin() + first_inside);
				}
			}
		}
		else // last range bound inside given range is a lower bound
		{
			if(first_inside > last_inside) // range is inside an internal range entirely
			{
				this->shard_ranges.insert(this->shard_ranges.begin() + first_inside, upper_bound + 1);
				this->shard_ranges.insert(this->shard_ranges.begin() + first_inside, lower_bound - 1);
				if(this->shard_ranges.at(first_inside) == this->shard_ranges.at(last_inside))
				{
					this->shard_singles.push_back(this->shard_ranges.at(first_inside));
					this->shard_ranges.erase(this->shard_ranges.begin() + last_inside, this->shard_ranges.begin() + first_inside);
				}
				if(this->shard_ranges.at(first_inside + 1) == this->shard_ranges.at(first_inside + 2))
				{
					this->shard_singles.push_back(this->shard_ranges.at(first_inside + 1));
					this->shard_ranges.erase(this->shard_ranges.begin() + first_inside + 1, this->shard_ranges.begin() + first_inside + 2);
				}
			}
			else // range is between two or more internal ranges
			{
				if(first_inside + 1 < last_inside)
					this->shard_ranges.erase(this->shard_ranges.begin() + first_inside + 1, this->shard_ranges.begin() + last_inside - 1);

				last_inside = first_inside + 1;
				this->shard_ranges.at(first_inside) = lower_bound - 1;
				this->shard_ranges.at(last_inside) = upper_bound + 1;
				if(this->shard_ranges.at(first_inside - 1) == lower_bound - 1)
				{
					this->shard_singles.push_back(lower_bound - 1);
					this->shard_ranges.erase(this->shard_ranges.begin() + first_inside - 1, this->shard_ranges.begin() + first_inside);
				}
				if(this->shard_ranges.at(last_inside + 1) == upper_bound + 1)
				{
					this->shard_singles.push_back(upper_bound + 1);
					this->shard_ranges.erase(this->shard_ranges.begin() + last_inside, this->shard_ranges.begin() + last_inside + 1);
				}
			}
		}
	}
	else // first range bound inside given range is a lower bound
	{
		if(last_inside & 1) // last range bound inside given range is an upper bound
		{
			if(last_inside > first_inside) // range is inside 1 or more internal ranges
			{
				this->shard_ranges.erase(this->shard_ranges.begin() + first_inside, this->shard_ranges.begin() + last_inside);
			}
		}
		else // last range bound inside given range is a lower bound
		{
			if(last_inside == first_inside) // only member in range is this lower bound
			{
				this->shard_ranges.at(first_inside) = upper_bound + 1;
				if(this->shard_ranges.at(first_inside + 1) == upper_bound + 1)
				{
					this->shard_singles.push_back(upper_bound + 1);
					this->shard_ranges.erase(this->shard_ranges.begin() + first_inside, this->shard_ranges.begin() + first_inside + 1);
				}
			}
			else
			{
				this->shard_ranges.erase(this->shard_ranges.begin() + first_inside, this->shard_ranges.begin() + last_inside - 1);
				this->shard_ranges.at(first_inside) = upper_bound + 1;
				if(this->shard_ranges.at(first_inside + 1) == upper_bound + 1)
				{
					this->shard_singles.push_back(upper_bound + 1);
					this->shard_ranges.erase(this->shard_ranges.begin() + first_inside, this->shard_ranges.begin() + first_inside + 1);
				}
			}
		}
	}
}

unsigned long* ShardManager::get_shard_singles(unsigned long& num_singles)
{
	num_singles = (unsigned long)this->shard_singles.size();

	if(this->return_array != nullptr)
		delete[] this->return_array;
	this->return_array = new unsigned long[num_singles];

	for(unsigned long i = 0; i < num_singles; i++)
		this->return_array[i] = this->shard_singles.at(i);

	return this->return_array;
}

unsigned long* ShardManager::get_shard_ranges(unsigned long& num_ranges)
{
	num_ranges = (unsigned long)this->shard_ranges.size() / 2;

	if(this->return_array != nullptr)
		delete[] this->return_array;
	this->return_array = new unsigned long[num_ranges * 2];

	for(unsigned long i = 0; i < num_ranges * 2; i++)
		this->return_array[i] = this->shard_ranges.at(i);

	return this->return_array;
}

void ShardManager::add_shard(unsigned long const shard_num, unsigned char const * const shard_data, unsigned short const shard_size)
{
	if(this->shard_available(shard_num) || !this->fill_mode)
		return;

	for(unsigned long i = 0; i < this->shard_singles.size(); i++)
	{
		if(this->shard_singles.at(i) == shard_num)
		{
			this->shard_singles.erase(this->shard_singles.begin() + i);
		}
	}

	unsigned long i = 0;
	while(i < this->shard_ranges.size() && this->shard_ranges.at(i) < shard_num)
	{
		i++;

		if(this->shard_ranges.at(i) > shard_num)
		{
			unsigned long next_range_end = this->shard_ranges.at(i);
			this->shard_ranges.at(i) = shard_num - 1;
			this->shard_ranges.insert(this->shard_ranges.begin() + i + 1, next_range_end);
			this->shard_ranges.insert(this->shard_ranges.begin() + i + 1, shard_num + 1);
			if(this->shard_ranges.at(i) == this->shard_ranges.at(i - 1))
			{
				this->shard_singles.push_back(shard_num - 1);
				this->shard_ranges.erase(this->shard_ranges.begin() + i - 1, this->shard_ranges.begin() + i);
			}
			if(this->shard_ranges.at(i + 1) == this->shard_ranges.at(i + 2))
			{
				this->shard_singles.push_back(shard_num + 1);
				this->shard_ranges.erase(this->shard_ranges.begin() + i + 1, this->shard_ranges.begin() + i + 2);
			}
		}
		else if(this->shard_ranges.at(i) == shard_num)
		{
			this->shard_ranges.at(i)--;
			if(this->shard_ranges.at(i) == this->shard_ranges.at(i - 1))
			{
				this->shard_singles.push_back(shard_num - 1);
				this->shard_ranges.erase(this->shard_ranges.begin() + i - 1, this->shard_ranges.begin() + i);
			}
		}

		i++;
	}

	if(this->shard_ranges.at(i) == shard_num)
	{
		this->shard_ranges.at(i) = shard_num + 1;
		if(this->shard_ranges.at(i) == this->shard_ranges.at(i + 1))
		{
			this->shard_singles.push_back(shard_num + 1);
			this->shard_ranges.erase(this->shard_ranges.begin() + i, this->shard_ranges.begin() + i + 1);
		}
	}

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
	bool exists_in_local_range = false;

	for(unsigned int i = 0; i < this->shard_singles.size(); i++)
		if(this->shard_singles.at(i) == shard_num)
			exists_in_local_range = true;

	for(unsigned int i = 0; i < this->shard_ranges.size(); i+=2)
		if(shard_num >= this->shard_ranges.at(i) && shard_num <= this->shard_ranges.at(i + 1))
			exists_in_local_range = true;

	if(this->fill_mode)
		return exists_in_local_range;
	else
		return !exists_in_local_range;
}

bool ShardManager::is_done()
{
	return this->shard_ranges.empty() && this->shard_singles.empty();
}

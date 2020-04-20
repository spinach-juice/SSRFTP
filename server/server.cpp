#include "communicator.h"
#include "shard_manager.h"
#include "packet.h"
#include <unordered_map>
#include "util.h"
#include <iostream>
#include <unistd.h>

std::vector<Message> next_shard_request;
bool shard_request_ready = false;

void handle_client_start(Message& m, std::unordered_map<unsigned short, ShardManager>& manager_map, std::unordered_map<unsigned short, char*>& md5_sums, Communicator& com)
{
	char md5_chk[32];
	unsigned long long file_size;
	unsigned long num_shards;
	unsigned short trans_id;
	char file_name[260];
	unsigned short name_len;
	Packet p = get_packet(m);

	if(interpret_client_start(p, md5_chk, file_size, num_shards, trans_id, file_name, name_len))
	{
		if(manager_map.count(trans_id) == 0)
		{
			ShardManager new_sm(file_name, trans_id, num_shards);

			char* new_md5 = new char[33];
			new_md5[32] = 0;
			for(unsigned char i = 0; i < 32; i++)
				new_md5[i] = md5_chk[i];

			manager_map[trans_id] = new_sm;
			md5_sums[trans_id] = new_md5;

			new_sm.disable(); // prevent duping manager
		}
		com.send_message(m);
	}
}

void handle_file_shard(Message& m, std::unordered_map<unsigned short, ShardManager>& manager_map, std::unordered_map<unsigned short, char*>& md5_sums, Communicator& com)
{
	Packet p = get_packet(m);
	unsigned long shard_num;
	unsigned short trans_id;
	unsigned char shard_data[65524];
	unsigned short shard_size;

	if(interpret_file_shard(p, shard_num, trans_id, shard_data, shard_size))
	{
		if(!(manager_map[trans_id].shard_available(shard_num)))
		{
			manager_map[trans_id].add_shard(shard_num, shard_data, shard_size);
			if(manager_map[trans_id].is_done())
			{
				std::string filename = manager_map[trans_id].get_filename();
				manager_map.erase(trans_id);

				char md5_chk[33] = {0};
				MD5(filename.c_str(), md5_chk);

				bool success_state;
				if(schar_array_equal(md5_chk, md5_sums[trans_id], 32))
					success_state = true;
				else
					success_state = false;

				Packet response = build_transfer_complete(trans_id, success_state);

				Message msg_response = package_message(response, get_endpoint(m));

				com.send_message(msg_response);
			}
		}
		else
		{
			// We already have this shard
			// Queue up another shard request to be sent

			unsigned long num_sing;
			unsigned long* sing = manager_map[trans_id].get_shard_singles(num_sing);
			unsigned long num_rang;
			unsigned long* rang = manager_map[trans_id].get_shard_ranges(num_rang);

			Packet rq = build_shard_request_range(trans_id, sing, num_sing, rang, num_rang);
			if(next_shard_request.empty())
				next_shard_request.push_back(package_message(rq, get_endpoint(m)));
			else
				next_shard_request.at(0) = package_message(rq, get_endpoint(m));
			shard_request_ready = true;
		}
	}
}

void handle_transfer_complete(Message& m, std::unordered_map<unsigned short, ShardManager>& manager_map, std::unordered_map<unsigned short, char*>& md5_sums, Communicator& com)
{

}

void handle_semi_robust(Message& m, std::unordered_map<unsigned short, ShardManager>& manager_map, std::unordered_map<unsigned short, char*>& md5_sums, Communicator& com)
{
	Packet p = get_packet(m);
	unsigned long shard_num;
	unsigned short trans_id;
	unsigned char shard_data[65524];
	unsigned short shard_size;
	bool final_shard;

	if(interpret_semi_robust_shard(p, trans_id, final_shard, shard_num, shard_data, shard_size))
	{
		if(manager_map.count(trans_id) == 0 && final_shard)
		{
			ShardManager new_sm("Semi-Robust_Transfer", trans_id, shard_num + 1);
			manager_map[trans_id] = new_sm;

			new_sm.disable(); // prevent duping shard manager

			manager_map[trans_id].add_shard(shard_num, shard_data, shard_size);
			if(manager_map[trans_id].is_done())
				manager_map.erase(trans_id);
		}
		else if(manager_map.count(trans_id) == 1 && !(manager_map[trans_id].shard_available(shard_num)))
		{
			manager_map[trans_id].add_shard(shard_num, shard_data, shard_size);
			if(manager_map[trans_id].is_done())
				manager_map.erase(trans_id);
		}
	}
}

int main()
{
	Communicator com;

	std::unordered_map<unsigned short, ShardManager> sms;
	std::unordered_map<unsigned short, char*> md5s;

	while(true)
	{
		unsigned char i = 0;
		while(com.message_available())
		{
			Message m = com.read_message();

			switch(get_packet(m).int_type())
			{
			case 0: // client start
				{
					handle_client_start(m, sms, md5s, com);
				}
			break;

			case 1: // file shard
				{
					handle_file_shard(m, sms, md5s, com);
				}
			break;

			case 3: // shard request
				{ // currently should never happen
				}
			break;

			case 4: // transfer complete
				{
					handle_transfer_complete(m, sms, md5s, com);
				}

			case 5: // semi-robust shard
				{
					handle_semi_robust(m, sms, md5s, com);
				}
			break;
			}

			if(i == 0)
			{
				if(shard_request_ready)
				{
					com.send_message(next_shard_request.at(0));
					shard_request_ready = false;
				}
			}
		}

		if(shard_request_ready)
		{
			com.send_message(next_shard_request.at(0));
			shard_request_ready = false;
		}

		usleep(100000);
	}
}



#include "communicator.h"
#include "shard_manager.h"
#include "packet.h"
#include <unordered_map>
#include "util.h"
#include <iostream>
#include <unistd.h>

void handle_client_start(Message& m, std::unordered_map<unsigned short, ShardManager>& manager_map, std::unordered_map<unsigned short, char*>& md5_sums, Communicator& com)
{std::cout << "recieved client start from " << get_endpoint(m) << std::endl;
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
{std::cout << "recieved file shard" << std::endl;
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
	}
}

void handle_transfer_complete(Message& m, std::unordered_map<unsigned short, ShardManager>& manager_map, std::unordered_map<unsigned short, char*>& md5_sums, Communicator& com)
{std::cout << "recieved transfer complete" << std::endl;
}

int main()
{
//    server test_server;
    
//    test_server.start_server();

	Communicator com;

	std::unordered_map<unsigned short, ShardManager> sms;
	std::unordered_map<unsigned short, char*> md5s;

	while(true)
	{
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
			break;
			}
		}

		usleep(10000);
	}
}



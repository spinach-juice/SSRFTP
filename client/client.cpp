#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <unistd.h>
#include "communicator.h"
#include "packet.h"
#include "util.h"
#include <cstring>
#include <cstdio>
#include "shard_manager.h"

int main(int argc, char** argv)
{
	// Command line arguments: ./SSRFTP_client filename endpoint
	if(argc != 3)
	{
		std::cout << "Usage: SSRFTP_client filename endpoint" << std::endl;
		return 1;
	}

	char file_to_transfer[261] = {0};
	strcpy(file_to_transfer, argv[1]);
	char endpoint[16] = {0};
	strcpy(endpoint, argv[2]);

	FILE* open_file = fopen(file_to_transfer, "r");
	if(open_file == nullptr)
	{
		std::cerr << "Could not open file: " << file_to_transfer << std::endl;
		return 1;
	}

	fseek(open_file, 0, SEEK_END);

	long file_size = ftell(open_file);
	fclose(open_file);

	unsigned long tot_shards = (unsigned long)file_size / 65495;
	if((unsigned long)file_size % 65495)
		tot_shards++;

	unsigned short trans_id = 1337;
	unsigned short path_length = (unsigned short)strlen(file_to_transfer);

	char md5_chk[33] = {0};
	MD5(file_to_transfer, md5_chk);
	std::cout << "client start packet:\nmd5 = " << md5_chk << "\nfile size = " << file_size << "\nnumber of shards = " << tot_shards << "\ntrans id = " << trans_id << "\nfile being transferred = " << file_to_transfer << std::endl;
	Packet client_start = build_client_start(md5_chk, file_size, tot_shards, trans_id, file_to_transfer, path_length);

	Communicator com;

	std::string rcv_endpoint;
	bool starting_handshake = false;
	while(!starting_handshake)
	{
		com.send_message(package_message(client_start, (std::string)endpoint));
		usleep(100000);
		while(com.message_available())
		{
			Message m = com.read_message();

			Packet p = get_packet(m);
			if(p == client_start)
			{
				rcv_endpoint = get_endpoint(m);
				starting_handshake = true;
			}
		}
	}
	while(com.message_available())
		com.read_message();

	ShardManager sm(file_to_transfer, trans_id);
	bool transfer_in_progress = true;
	while(transfer_in_progress)
	{
		// send range of file shards
		for(unsigned long i = 0; i < tot_shards; i++)
		{
			if(sm.shard_available(i))
			{
				unsigned char* shard_data;
				unsigned short shard_size;

				shard_data = sm.get_shard_data(i, shard_size);

				Packet p = build_file_shard(i, trans_id, shard_data, shard_size);
				com.send_message(package_message(p, (std::string)endpoint));
			}
		}

		// Sleep for a bit
		usleep(100000);

		// Check for a response
		while(com.message_available())
		{
			Packet p = get_packet(com.read_message());

			switch(p.int_type())
			{
			case 3: // shard request packet
				{
					unsigned short recv_trans_id;
					std::vector<unsigned long> missing_singles;
					std::vector<unsigned long> missing_ranges;
					if(interpret_shard_request_range(p, recv_trans_id, missing_singles, missing_ranges) && recv_trans_id == trans_id)
					{
						// narrow shards being sent
					}
				}
			break;

			case 4: // transfer complete packet
				{
					unsigned short recv_trans_id;
					bool success;
					if(interpret_transfer_complete(p, recv_trans_id, success) && recv_trans_id == trans_id && success)
					{
						transfer_in_progress = false;
						com.send_message(package_message(p, (std::string)endpoint));
					}
				}
			break;
			}
		}
	}
	return 0;
}

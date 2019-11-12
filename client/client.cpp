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
#include "client.h"
#include "util.h"
#include "tcp_listener.h"

unsigned long const DataPerPacket = 1024;

int state = 0; 
bool state_change = false;
std::vector<Packet> shardPackets;
Packet return_packet = nullptr;
Communicator com; 

int main(int argc, char** argv)
{
	
	
	std::ifstream* file;	
	unsigned long long fileSize;
	unsigned long shard_num;
	unsigned short trans_id = 0;
	char const* destination_path = "some path";
	unsigned short path_length = 0;
	pthread_t send_loop;
	pthread_t receive_loop;
	
	tcpListener full_file;
	full_file.Listen();
	file = &full_file.getPath();
	fileSize = getFileSize(file);
	
	shard_num = fileSize/DataPerPacket;
	
	if(fileSize % DataPerPacket != 0)
		shard_num++;

	//assume that this works for now
	char file_checksum[33]; 
	MD5(file,file_checksum); 


	Packet start_packet = build_client_start(file_checksum,fileSize,shard_num,trans_id,destination_path,path_length); 


	com.start();
	
	while(state == 0)
	{
		if(com.message_available())
		{
			Message m = com.read_message();
			if(m.first == start_packet)
				state = 1;
		}
	
		com.send_message(package_message(start_packet,""));
	}
	
	Packet current = nullptr;
	char data[DataPerPacket];
	
	for(int i = 0; i< shard_num; i++)
	{
		file->read(data, sizeof(data-1));
		current = build_file_shard(i, /*trasmittion id*/5, (unsigned char const * const)data, DataPerPacket);
		shardPackets.push_back(current);
	}

	pthread_create(&send_loop, nullptr,send,nullptr);
	pthread_create(&receive_loop,nullptr,receive, nullptr);

	//wait for the server to send the request packet
	while(sizeof(return_packet) == 0)
	{
		usleep(1);
	}
	
	unsigned long missing_shards[shard_num];
	unsigned long  num_missing_shards;
	
	
	interpret_shard_request(return_packet,trans_id, missing_shards, num_missing_shards);

 	std::vector<Packet> new_shardPackets;
	//change the packets that are being sent to only the ones we need
	for(int i = 0; i < num_missing_shards;i++)
		new_shardPackets.pushback(shardPackets[missing_shards[i]]);

	
	
  return 0;
}


void* send(void* args)
{

	while(!state_change)
	{
		for(int i = 0; i < sizeof(shardPackets); i++)
			com.send_message(package_message(shardPackets[i],""));
		
	}
		

}
void* receive(void* args)
{
	while(!state_change)
	{
		if(com.message_available())
		{
			Message m = com.read_message();
			return_packet = m.first;
			state_change = 1;
		}
		
	}
	

}

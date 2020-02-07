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
Communicator com; 

int main(int argc, char** argv)
{
	
	std::ifstream file;	
	unsigned long long fileSize;
	unsigned long shard_num;
	unsigned short trans_id = 0;
	char const* destination_path = "file";
	unsigned short path_length = 4;
	pthread_t send_loop;
	pthread_t receive_loop;


	if(argc < 2)
	{
		std::cout << "Filename not provided, please provide filename." << std::endl;
		return 0;
	}
	

	file.open(argv[1]);	

	fileSize = getFileSize(file);
	
	shard_num = fileSize/DataPerPacket;
	
	if(fileSize % DataPerPacket != 0)
		shard_num++;

	//assume that this works for now
	char file_checksum[33]; 
	char buffer[fileSize];
	getFileContents(file,fileSize, buffer);
	
	MD5(argv[1],file_checksum); 
	
	
	
	Packet start_packet = build_client_start(file_checksum,fileSize,shard_num,trans_id,destination_path,path_length); 
	
	com.start();
	/*
	while(state == 0)
	{
		if(com.message_available())
		{
			Message m = com.read_message();
			if(m.first == start_packet)
				state = 1;
		}
	std:: cout << "SENDING" << std::endl;	com.send_message(package_message(start_packet,"192.168.1.1"));
		usleep(1000000);
	} 
	
*/
	char data[DataPerPacket];
	Packet current = build_file_shard(0, /*trasmittion id*/5, (unsigned char const * const)data, DataPerPacket);
	
	for(int i = 0; i< (int)shard_num; i++)
	{
		
		file.read(data, (int)sizeof(data));
		current = build_file_shard(i, /*trasmittion id*/5, (unsigned char const * const)data, DataPerPacket);
		shardPackets.push_back(current);
	}

	pthread_create(&send_loop, nullptr,send,nullptr);
	pthread_create(&receive_loop,nullptr,receive, nullptr);

	//wait for the server to send the request packet
	while(!com.message_available())
	{
		usleep(100000);
	}

	Message m = com.read_message();
	Packet return_packet = m.first;
	
	unsigned long missing_shards[shard_num];
	unsigned long  num_missing_shards;
	std::vector<Packet> new_shardPackets;

	//this will need to be implemented into a function later
	//to 1)clean up main 2)the have steps in process be defined
	
	bool transferComplete = false;
	bool success_state = true;
	while(!transferComplete)
	{
		interpret_shard_request(return_packet,trans_id, missing_shards, num_missing_shards);
		Packet transfer_complete_packet  = build_transfer_complete(/*trasmittion id*/5, success_state);

	 	
		//change the packets that are being sent to only the ones we need
		for(int i = 0; i < (int)num_missing_shards;i++)
			new_shardPackets.push_back(shardPackets[missing_shards[i]]);

		shardPackets = new_shardPackets;
		
		state_change = 0; 
		pthread_create(&send_loop, nullptr,send,nullptr);
		pthread_create(&receive_loop,nullptr,receive, nullptr);
		

		
		
		while(!state_change)
		{
			usleep(1000000);
		}
		
		Message m = com.read_message();
		if(m.first == transfer_complete_packet)
			transferComplete = true;
		else 
			new_shardPackets.clear();
	}
	
  	return 0;
}


void* send(void* args)
{
	
	std::cout << (int)sizeof(shardPackets) << std::endl;
	while(!state_change)
	{
		for(int i = 0; i < (int)sizeof(shardPackets); i++)
		{
		std::cout << i << std::endl;	com.send_message(package_message(shardPackets[i],"192.168.1.1"));
			usleep(1000000);
		}
	}
		

}
void* receive(void* args)
{
	while(!state_change)
	{
		if(com.message_available())
		{
			state_change = 1;
		}
		usleep(100000);
		
	}

}

void getDataPath()
{

}
void getMD5checksum()
{

}
void sendStart()
{

}
void sendData()
{

}






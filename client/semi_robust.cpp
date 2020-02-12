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

std::vector<Packet> shardPackets;
Communicator com;
const int numberOfTransmittions = 3;

int main(int argc, char** argv)
{

	std:string filename;
	unsigned long long fileSize;
	unsigned long shard_num;
	unsigned short trans_id = 0;
	char const* destination_path = "file";
	unsigned short path_length = 4;
	pthread_t send_loop;
	pthread_t receive_loop;
	

	if(argc != 2)
	{
		std::cout << "Filename not provided, please provide filename." << std::endl;
		return 0;
	}


	filename = argv[0];
	std::ifstream file;

	try
	{
		file.open(argv[0])	
	}
	catch(std::string message)
	{
		std::cout<< message << ", please check to ensure file exists" << std::endl;
		return 0;
	}

	fileSize = getFileSize(file);
	shard_num = fileSize/DataPerPacket;
	
	if(fileSize % DataPerPacket != 0)
		shard_num++;

	com.start();
	char data[DataPerPacket];
	Packet current = build_file_shard(0, /*trasmittion id*/5, (unsigned char const * const)data, DataPerPacket);
	
	for(int i = 0; i< (int)shard_num; i++)
	{
		
		file.read(data, sizeof(data));
		current = build_file_shard(i, /*trasmittion id*/5, (unsigned char const * const)data, DataPerPacket);
		shardPackets.push_back(current);
	}

	for(int j = 0; j < numberOfTransmittions; j++)
	{
		for(int i = 0; i < (int)sizeof(shardPackets); i++)
		{
			com.send_message(package_message(shardPackets[i],argv[1]));
			usleep(10000);
		}
	}

}



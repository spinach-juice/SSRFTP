#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <string>
#include <queue>

#include "packet.h"
#include "client.h"
#include "util.h"
#include "tcp_listener.h"

unsigned long const shardsPerFile = 1024;
std::queue<Packet> send_packet_queue;
std::queue<Packet> receive_packet_queue;


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
	pthread_t state_loop;
	
	tcpListener full_file;
	full_file.Listen();
	file = full_file.getPath();
	fileSize = getFileSize(file);
	
	shard_num = fileSize/shardsPerFile;
	
	if(fileSize % shardsPerFile != 0)
		shard_num++;


	char file_checksum[33] = {*MD5(file)}; //this will NOT work by the way


	Packet start_packet = build_client_start(file_checksum,fileSize,shard_num,trans_id,destination_path,path_length); 

	

	send_packet_queue.push(start_packet);

	pthread_create(&send_loop,NULL,send,NULL);
	pthread_create(&receive_loop,NULL,receive,NULL);

	
	

	
	
  return 0;
}


void *send()
{
	if(send_packet_queue.size() != 0)
	{
		//send the file using udp

	}	

}
void *receive()
{
	

}

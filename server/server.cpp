#include "server.h"

using namespace std;

server::server() //TODO - change comm port and endpoint from default
{
    
}

server::~server()
{
    this->kill();
}
void server::start_server()
{
    server_comm = new Communicator();
    bool transfer_complete = false;
    
    while(!transfer_complete)
    {
        string filename;
            
        if(server_comm->message_available())
        {   
            
            /*-------current data storage variables for packets-----*/
            // common
            unsigned short trans_id;
            
           // client start packet params
            char* md5_chksum = nullptr;
            unsigned long long file_size;
            unsigned long num_shards;
            char* destination_path = nullptr; // to be cancelled
            unsigned short path_length;
            
            // normal shard parameters
            unsigned long shard_num;
            unsigned char* shard_data = nullptr;
            unsigned short data_size;
            
            // temp ifstream object to check if a file already exists
            fstream repeat_checker;
                
            ShardChecker chek;
            vector <unsigned long> missing_shards;
            unsigned long* miss_shard_arr;
            Packet curr = get_packet(server_comm->read_message());
            
            switch(curr.int_type())
            {
                case(0):
                    if(interpret_client_start(curr, md5_chksum, 
                        file_size, num_shards, trans_id, 
                            destination_path, path_length))
                    {
                        filename = "shard/clientstart.shrd";
                        repeat_checker.open(filename, ios::in);
                        
                        // checks if file exits, if not creates one and inputs
                        if(!repeat_checker.is_open())
                        { 
                         
                            // do I print out different variants of file sizes?
                            if(md5_chksum != nullptr)
                            {
                                file.open(filename, ios::out | ios::trunc);
                                file << md5_chksum << endl
                                    << file_size << endl
                                    << num_shards << endl
                                    << trans_id << endl
                                    << path_length << endl
                                    << destination_path << endl;
                                file.close();
                            } else
                            {
                                cout << "Client start packet MD5 Checksum " 
                                    << "missing.\n"
                                    << "Critical error. Exiting..." << endl;
                                return;
                            }
                        }else 
                        {
                            chek.extractClientStartPacket();
                            missing_shards = chek.verifyShards();
                        }
                        
                        repeat_checker.close();
                    }
                    break;
                
                // normal shard packet
                case(1):
                    if(interpret_file_shard(curr, shard_num,
                        trans_id, shard_data, data_size))
                    {
                        filename = "shard/" + to_string(shard_num)
                            +".shrd";
                        repeat_checker.open(filename, ios::in);
                        
                        
                        if(!repeat_checker.is_open())
                        {
                            file.open(filename, ios::out | ios::trunc);
                            file << shard_num << endl 
                                << trans_id << endl
                                << data_size << endl
                                << shard_data << endl; 
                            file.close();
                        }
                        else 
                        {
                            chek.extractClientStartPacket();
                            missing_shards = chek.verifyShards();
                        }
                        file.open(filename, ios::out | ios::trunc);
                        file.close();
                    }
                    break;
                    
                default:
                    break;
            }

            //This part calls functions to build the Shard Request Packet
            if(!missing_shards.empty())
            {  
                miss_shard_arr = new unsigned long[missing_shards.size()];
                for(unsigned long i = 0; i < missing_shards.size(); i ++)
                    miss_shard_arr[i] = missing_shards[i];
                
                server_comm->send_message(
                    Message(build_shard_request(7 /*trans id*/, 
                        miss_shard_arr, missing_shards.size())
                        , "192.168.1.1"));
            }
            else
            {
                cout << "Transfer Complete!" << endl << endl;
                transfer_complete = true;
                
                sleep(100000);
                //send request complete packet
                
            }

            sleep(200);
        } 
    }
    
    
    return;
}

void server::kill()
{
    //server_comm->kill();
}

#include "communicator.h"
#include "shard_manager.h"
#include "packet.h"
#include <unordered_map>
#include "util.h"

void handle_client_start(Message& m, std::unordered_map<unsigned short, ShardManager>& manager_map, std::unordered_map<unsigned short, char*> md5_sums, Communicator& com)
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
		}
		com.send_message(m);
	}
}

void handle_file_shard(Message& m, std::unordered_map<unsigned short, ShardManager>& manager_map, std::unordered_map<unsigned short, char*> md5_sums, Communicator& com)
{
	Packet p = get_packet(m);
	unsigned long shard_num;
	unsigned short trans_id;
	unsigned char shard_data[65524];
	unsigned short shard_size;

	if(interpret_file_shard(p, shard_num, trans_id, shard_data, shard_size))
	{
		if(!manager_map[trans_id].shard_available(shard_num))
		{
			manager_map[trans_id].add_shard(shard_num, shard_data, shard_size);
			if(manager_map[trans_id].is_done())
			{
				std::string filename = manager_map[trans_id].get_filename();
				manager_map.erase(trans_id);

				char md5_chk[32];
				MD5(filename.c_str(), md5_chk);

				bool success_state;
				if(schar_array_equal(md5_chk, md5_sums[trans_id], 32))
					success_state = true;
				else
					success_state = false;

				Packet response = build_transfer_complete(trans_id, success_state);

				Message msg_response = package_message(response, get_endpoint(m));
			}
		}
	}
}

void handle_transfer_complete(Message& m, std::unordered_map<unsigned short, ShardManager>& manager_map, std::unordered_map<unsigned short, char*> md5_sums, Communicator& com)
{
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



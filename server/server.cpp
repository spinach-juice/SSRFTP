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
    bool server_send = false;
    
    while(!server_send)
    {
        string filename;
        
        if(server_comm->message_available())
        {   
        
            /*-------current data storage variables for packets-----*/
            // common
            unsigned short trans_id;
            
            // client start packet params
            char* md5_chksum;
            unsigned long long file_size;
            unsigned long num_shards;
            char* destination_path;
            unsigned short path_length;
            
            // normal shard parameters
            unsigned long shard_num;
            unsigned char* shard_data;
            unsigned short data_size;
            
            Packet curr = get_packet(server_comm->read_message());
            
            switch(curr.int_type())
            {
                // client start packet
                case(0):
                    if(interpret_client_start(curr, md5_chksum, 
                        file_size, num_shards, trans_id, 
                            destination_path, path_length))
                    {
                        filename = "shard/clientstart.shrd";
                        
                        // do I print out different variants of file sizes?
                        file << md5_chksum << endl
                            << file_size << endl
                            << num_shards << endl
                            << trans_id << endl
                            << path_length << endl
                            << destination_path << endl;
                    }
                    break;
                
                // normal shard packet
                case(1):
                    if(interpret_file_shard(curr, shard_num,
                        trans_id, shard_data, data_size))
                    {
                        filename = "shard/" + to_string(shard_num)
                            +".shrd";
                        file.open(filename, ios::out | ios::trunc);
                        file << shard_num << endl 
                            << trans_id << endl
                            << data_size << endl
                            << shard_data << endl; 
                            
                        file.close();
                    }
                    break;
                    
                default:
                    break;
            }
        } 
    }
    
}

void server::kill()
{
    server_comm->kill();
}

int main()
{
    server test_server;
    
    //server.start_server();
}



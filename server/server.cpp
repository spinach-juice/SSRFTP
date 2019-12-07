#include "server.h"

using namespace std;

server::server()
{

}

server::~server()
{
    //this->kill();
}

void server::start_server()
{
    Communicator server_comm;
    ShardManager* shard_manager = nullptr;
    
    string client_ip = "151.159.105.38";
    
    bool client_start_packet_received = false;
    bool repeat = false;
    
    // common packet params
    unsigned short trans_id;
    
    // client start packet params
    char md5_chksum[33] = {0};
    unsigned long long file_size;
    unsigned long num_shards;
    char filename[261] = {0};
    unsigned short filename_length;
    
    // general data shard params
    unsigned long shard_num;
    unsigned char shard_data[SHARD_SIZE_MAX + 1] = {0};
    unsigned short data_size;
      
    while(true)
    {
        if(server_comm.message_available())
        {
            Packet current_packet = get_packet(
                                        server_comm.read_message());
                    
            if(current_packet.int_type() == 0)
            {
                // dbg
                cout << "Received client start packet" << endl;
                // end-dbg
                
                if(!interpret_client_start(current_packet, md5_chksum, 
                            file_size, num_shards, trans_id, 
                                filename, filename_length))
                {  
                    //dbg
                    cout << "******Corrupted Client Start"
                        << "Packet received******" << endl;
                    //end-dbg
                    
                    //TODO - log error 
                    break;
                }
                else
                {
                    if(shard_manager == nullptr)
                    {
                        shard_manager = new ShardManager(filename,
                                        trans_id, num_shards);
                                        
                        fstream ofile("shard/clientstartpacket.shrd",
                            ios::out | ios::trunc);
                            
                        if(ofile.is_open())
                            ofile << md5_chksum << endl
                                << file_size << endl
                                << num_shards << endl
                                << trans_id << endl
                                << filename << endl
                                << filename_length << endl;
                                
                        server_comm.send_message(package_message(
                            current_packet, client_ip));
                    }
                    else
                        repeat = true;
                }
            }      
                          
            else if(client_start_packet_received == true
                && current_packet.int_type() == 1)
            {
                //dbg
                cout << "Received file shard" << endl;
                //end-dbg
                
                if(!interpret_file_shard(current_packet, shard_num,
                            trans_id, shard_data, data_size))
                {
                    //dbg
                    cout << "******Corrupted Shard"
                        << "Packet received******" << endl;
                    //end-dbg
                    
                    //TODO - log error 
                    break;
                }
                else
                {
                    if(!shard_manager->shard_available(shard_num))
                        shard_manager->add_shard(shard_num, shard_data,
                            data_size);
                    else
                        repeat = true;
                }
            }
            
            if(repeat == true)
            {
                unsigned long num_singles;
                unsigned long num_ranges;
                
                    server_comm.send_message(
                    package_message(
                    build_shard_request_range(trans_id,
                        shard_manager->get_shard_singles(num_singles),
                        num_singles, 
                        shard_manager->get_shard_ranges(num_ranges),
                        num_ranges), client_ip)); 
            }
            
            repeat = false; 
        }  
    }
}

int main()
{
    server test_server;
    
    test_server.start_server();
}

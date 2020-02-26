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
    
    string client_ip = "192.168.0.125";
    
    bool client_start_packet_received = false;
    bool transfer_complete = false;
    bool shard = false;
    
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
      
    while(!transfer_complete)
    {
        // check if a packet is available to process
        if(server_comm.message_available())
        {
            Packet current_packet = get_packet(
                                        server_comm.read_message());
                                        
                                        
   /**************** Client Start Packet Handling ***************************       
    This part handles getting and processing the client start   
    packet. It then stores it locally as clientstartpacket.shrd 
    This local copy contains data like the total number of packets
    and transfer id. This file is then deleted when the transfer 
    is complete.*/    
            if(current_packet.int_type() == 0)
            {
                // dbg
                cout << "Received client start packet" << endl;
                // end-dbg
                
                // computes the checksum to see if the packet is not corrupted
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
                        
                        cout << "\nSending back Client Start Packet\n";       
                        server_comm.send_message(package_message(
                            current_packet, client_ip));
                    }
                }
            }      
             
             
       /******************** Normal Packet Handling ********************/             
            else if(current_packet.int_type() == 1)
            {
                if(shard_manager == nullptr){
                        fstream ifile("shard/clientstartpacket.shrd",
                                ios::in);
                        if(ifile.is_open()){
                            ifile >> md5_chksum  
                                    >> file_size
                                    >> num_shards
                                    >> trans_id
                                    >> filename 
                                    >> filename_length;
                            shard_manager = new ShardManager(filename,
                                            trans_id, num_shards);
                        }
                    }
                        
                shard = true;
                //dbg
                cout << "Received file shard" << endl;
                //end-dbg
                
                // compute checksum to check if packet is corrupted
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
                    if(!shard_manager->shard_available(shard_num))
                        shard_manager->add_shard(shard_num, shard_data,
                            data_size);
            }
            
        
            if(shard_manager != nullptr && shard == true)
            {
                unsigned long num_singles;
                unsigned long num_ranges;
                unsigned long* singles = shard_manager->get_shard_singles(num_singles);
                unsigned long* ranges = shard_manager->get_shard_ranges(num_ranges);
                
                //dbg
                cout << "Sending Shard Request Packet\n";
                //end-dbg
                
                if(num_singles != 0 || num_ranges != 0)
                    server_comm.send_message(
                    package_message(
                    build_shard_request_range(trans_id,
                        singles,
                        num_singles, 
                        ranges,
                        num_ranges), client_ip)); 
                        
                if(shard_manager->is_done())
                    transfer_complete = true;
                //dbg
                cout << shard_manager->is_done() << endl << endl;
                //end-dbg
                
            }
        }  
    }
}

int main()
{
    server test_server;
    
    test_server.start_server();
    
    std::cout << "\nTransfer Complete\n";
}

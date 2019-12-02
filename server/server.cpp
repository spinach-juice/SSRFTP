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
<<<<<<< HEAD
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
=======
    bool server_send = false;
    
    while(!server_send)
    {
        string filename;
        Packet curr;
        
        if(server_comm->message_available())
        {   
            // client start packet params
            char* md5_chksum;
            unsigned long long file_size;
            unsigned long num_shards;
            unsigned short trans_id;
            char* destination_path;
>>>>>>> 5f99fe1dabfa9b13a7fe687b370cafacd8469ffa
            unsigned short path_length;
            
            // normal shard parameters
            unsigned long shard_num;
<<<<<<< HEAD
            unsigned char* shard_data = nullptr;
            unsigned short data_size;
            
            // temp ifstream object to check if a file already exists
            fstream repeat_checker;
                
            ShardChecker chek;
            vector <unsigned long> missing_shards;
            unsigned long* miss_shard_arr;
            Packet curr = get_packet(server_comm->read_message());
            
            //switch(1)
            switch(curr.int_type())
            {
                // client start packet
=======
            unsigned short trans_id;
            unsigned char* shard_data;
            unsigned short data_size;
            
            curr = server_comm->read_message().get_packet();
            
            switch(curr.int_type())
            {
>>>>>>> 5f99fe1dabfa9b13a7fe687b370cafacd8469ffa
                case(0):
                    if(interpret_client_start(curr, md5_chksum, 
                        file_size, num_shards, trans_id, 
                            destination_path, path_length))
<<<<<<< HEAD
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
                                    << " Critical error. Exiting..." << endl;
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
=======
                    break;
                
                case(1)
>>>>>>> 5f99fe1dabfa9b13a7fe687b370cafacd8469ffa
                    if(interpret_file_shard(curr, shard_num,
                        trans_id, shard_data, data_size))
                    {
                        filename = "shard/" + to_string(shard_num)
                            +".shrd";
<<<<<<< HEAD
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
=======
                        file.open(filename, ios::out | ios::trunc);
                        file.close();
>>>>>>> 5f99fe1dabfa9b13a7fe687b370cafacd8469ffa
                    }
                    break;
                    
                default:
                    break;
            }
<<<<<<< HEAD
            
            
            //This part calls functions to build the Shard Request Packet
            if(true)
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
            }
 
=======
>>>>>>> 5f99fe1dabfa9b13a7fe687b370cafacd8469ffa
        } 
    }
    
    
    return;
}

void server::kill()
{
<<<<<<< HEAD
    //server_comm->kill();
}

int main()
{
    server test_server;
    
    test_server.start_server();
=======
    server_comm->kill();
>>>>>>> 5f99fe1dabfa9b13a7fe687b370cafacd8469ffa
}



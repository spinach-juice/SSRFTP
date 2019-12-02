#include "ShardChecker.h"

ShardChecker::ShardChecker()
{

}

bool ShardChecker::extractClientStartPacket()
{
    fstream in;
    in.open("shard/clientstart.shrd", ios::in);
    
    if(!in.is_open())
    {
        cout << "Missing Client Start Packet" << endl << endl;
        return false;   
    }
    
    try{
        // get the md5_chksum
        in.getline(md5_chksum, 128);
        
        // skip 11 bits (the filesize entry + \n);
        in.seekg(10, ios::cur);
        
        char* temp;
        in.getline(temp, 10);
        num_shards = atoi(temp);
    }catch(...){
        cout << "Exception occured while trying to extract data from Client"
            << " Start Packet" << endl;
    }
    
    return true;
}

vector<unsigned long> ShardChecker::verifyShards()
{
    fstream in;
    vector<unsigned long> miss_shrd;
    
    for(unsigned long i = 0; i < num_shards; num_shards++)
    {
        string filename = "shard/" + i;
        filename = filename + ".shrd";
        in.open(filename, ios::in);
        
        if(!in.is_open())
            miss_shrd.push_back(i);
    }   
            
   return miss_shrd;
}



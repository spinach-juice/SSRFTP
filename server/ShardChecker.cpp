#include "ShardChecker.h"

bool ShardChecker::extractClientStartPacket()
{
    fstream in;
    
    if(!in.open("shard/clientstart.shrd", ios::in))
    {
        cout << "Missing Client Start Packet" << endl << endl;
        return false;   
    }
    
    try{
        // get the md5_chksum
        in.getline(md5_chksum, 128);
        
        // skip 11 bits (the filesize entry + \n);
        long pos = in.tellg();
        in.seekg(10, pos);
        
        char* temp;
        in.getline(temp, 10);
        num_shards = atoi(temp);
    }catch(...){
        cout << "Exception occured while trying to extract data from Client"
            << " Start Packet" << endl;
    }
    
    return true;
}

vector ShardChecker::verifyShards()
{
    fstream in;
    vector<int> miss_shrd [64];
    
    for(int i = 0; i < num_shards; num_shards+)
        if(!in.open("shard/" + i + ".shrd", ios::in))
            miss_shrd.push_back(i);
            
    return miss_shrd;
]



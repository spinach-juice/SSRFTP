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
        in.getline(md5_chksum, 33);
        
        in >> num_shards;
        //in.getline((char*)&num_shards, 4);
        
        //Debug
        cout << "md5chksum: " << md5_chksum << endl;
        cout << "Number of shards: " << num_shards << endl;
        //end-dbg   
        
        in.close();
        
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
    
    if(!this->extractClientStartPacket())   
    // debug
       cout << "Could not open client start packet for checking" << endl; 
   // enddbg
    
    for(unsigned long i = 0; i < num_shards; num_shards++)
    {
        string filename = "shard/" + i;
        filename = filename + ".shrd";
        in.open(filename, ios::in);
        
        if(!in.is_open())
            miss_shrd.push_back(i);
    }   
    
   //Debug
   cout << "Size of vector after check: " << miss_shrd.size() << endl << endl; 
   // end-dbg     
            
   return miss_shrd;
}



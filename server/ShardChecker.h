#ifndef _SHARD_CHECKER
#define _SHARD_CHECKER

#include "communicator.h"
#include "packet.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class ShardChecker
{
private:
    
    // client start packet data
    char* md5_chksum = nullptr;
    unsigned long num_shards;
  

public:
    ShardChecker();
    bool extractClientStartPacket();
    vector<unsigned long> verifyShards();
};

#endif

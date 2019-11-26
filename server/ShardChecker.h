#ifndef _SHARD_CHECKER
#define _SHARD_CHECKER

#include "communicator.h"
#include "packet.h"
#include <fstream>
#include <vector>

class ShardChecker
{
private:
    
    // client start packet data
    char* md5_chksum = nullptr;
    unsigned long num_shards;
  

public:
    bool extractClientStartPacket();
    vector verifyShards();
};

#endif

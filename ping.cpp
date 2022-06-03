#include "ping.h"

#include <iostream>
#include <limits>

unsigned short checksum(void *b, int len)
{    
    unsigned short *buf = (unsigned short *) b;
    unsigned int sum=0;
    unsigned short result;
 
    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

Ping::Ping(std::map<std::uint32_t, PingConfig> addrCfgs)
{
    this->addrCfgs = addrCfgs;
}

bool Ping::Init() 
{
    for (const auto& kv : this->addrCfgs) {

        addrToSockAddr[kv.second.IP];//.insert(std::pair<std::string, sockaddr_in>(kv.second.IP, sockaddr_in()));
        addrToSockAddr[kv.second.IP].sin_family = AF_INET;
        addrToSockAddr[kv.second.IP].sin_addr.s_addr = inet_addr(kv.second.IP.c_str());
        
        if ((addrToSock[kv.second.IP] = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
            return false;
        }
    }

    return true;
}

Ping::~Ping() 
{

}
#include "ping.h"

Ping::Ping(std::map<std::uint32_t, PingConfig> addrCfgs)
{
    this->addrCfgs = addrCfgs;
}

bool Ping::Init() 
{
    for (const auto& kv : this->addrCfgs) {

        addrToSockAddr[kv.second.IP];//.insert(std::pair<std::string, sockaddr_in>(kv.second.IP, sockaddr_in()));
        // std::cout << kv.first << " has value " << kv.second << std::endl;
        
        // TODO: Setting addr
    }
    return true;
}

Ping::~Ping() 
{

}
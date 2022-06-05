#include "ping.h"

#include <cstring>
#include <iostream>
#include <limits>
#include <netinet/ip.h>


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

std::map<std::uint32_t, PingConfig> Ping::Exec() 
{
    PingPkg pkg;
    bzero(&pkg, sizeof(pkg));

    PingPkgRecv recvPkg;
    bzero(&recvPkg, sizeof(recvPkg));

    Counter counter;
    int fromlen;
    uint32_t msg = 1;
    
    pkg.hdr.type = ICMP_ECHO;
    pkg.hdr.un.echo.id = getpid();
    pkg.setNbAsMsg(msg);

    counter++;
    pkg.hdr.un.echo.sequence = counter.nb;
    counter++;
    pkg.hdr.checksum = checksum(&pkg, sizeof(pkg));

    sockaddr_in s_addr = this->addrToSockAddr["173.194.222.101"];
    int s = this->addrToSock["173.194.222.101"];

    if ( 
        sendto(
            s, 
            &pkg, 
            sizeof(pkg), 
            0, 
            (struct sockaddr*) 
            &s_addr, 
            (socklen_t)sizeof(sockaddr_in)
            ) <= 0
        ) {
            printf("\nPacket Sending Failed!\n");
        }

    fromlen = sizeof(sockaddr_in);
    if ( 
        recvfrom(
            s, 
            &recvPkg, 
            sizeof(recvPkg), 
            0,
            (struct sockaddr*)&s_addr, 
            (socklen_t*)&fromlen
            ) <= 0
        ) {
            printf("\nPacket receive failed!\n");
        }

    std::cout << *((uint32_t*) recvPkg.ping_pkg.msg) << std::endl;

    return this->addrCfgs;
    
}

Ping::~Ping() 
{

}
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

std::map<std::uint32_t, PingConfig> Ping::Exec() 
{
    PingPkg pkg;
    // pkg.setNbAsMsg(1);
    // std::cout << *((uint32_t*)pkg.msg) << std::endl;
    // pkg.setNbAsMsg(2);
    // std::cout << *((uint32_t*)pkg.msg) << std::endl;

    Counter counter;
    int fromlen;
    uint32_t msg = 2643460096;
    
    bzero(&pkg, sizeof(pkg));
    pkg.hdr.type = ICMP_ECHO;
    pkg.hdr.un.echo.id = getpid();

    pkg.setNbAsMsg(msg);
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
    std::cout << "before " << *((uint32_t*)(pkg.msg)) << std::endl;

    PingPkg recvPkg;

    bzero(&recvPkg, sizeof(recvPkg));
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

    std::cout << "after " << *((int*)recvPkg.msg) << std::endl; // TODO: resolve

    return this->addrCfgs;
    
}

Ping::~Ping() 
{

}
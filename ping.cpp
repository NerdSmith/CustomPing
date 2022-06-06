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

    SeqCounter seqCounter;
    int fromlen;
    std::set<uint32_t> toSend;

    // debug vars
    uint32_t msg = 1;
    // debug vars end

    for(std::map<std::uint32_t, PingConfig>::iterator it = this->addrCfgs.begin(); it != this->addrCfgs.end(); ++it) {
        toSend.insert(it->first);
    }

    std::set<uint32_t>::iterator er;
    // toSend.erase(er);

    fd_set sockets_fds;

    std::string currIP;
    while (!toSend.empty()) {
        FD_ZERO(&sockets_fds);

        for (std::set<uint32_t>::iterator it = toSend.begin(); it != toSend.end(); it++) {
            currIP = this->addrCfgs[*it].IP;
            std::cout << "ip adding: " << currIP << std::endl; 
            FD_SET(addrToSock[currIP], &sockets_fds);
        }




        er = toSend.begin();
        std::cout << "rm: " << *er << std::endl;
        toSend.erase(er);
    }

    pkg.hdr.type = ICMP_ECHO;
    // std::cout << "pid: " << getpid() << " " << std::endl;
    pkg.hdr.un.echo.id = getpid() % std::numeric_limits<u_int16_t>::max();
    std::cout << "pid echo: " << pkg.hdr.un.echo.id << " " << std::endl;
    pkg.setNbAsMsg(msg);

    pkg.hdr.un.echo.sequence = seqCounter.nb;
    seqCounter++;
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

    // std::cout << *((uint32_t*) recvPkg.ping_pkg.msg) << std::endl;

    return this->addrCfgs;
}

Ping::~Ping() 
{

}
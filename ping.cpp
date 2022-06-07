#include "ping.h"

#include <cstring>
#include <iostream>
#include <limits>
#include <netinet/ip.h>

void printBytes(PingPkgRecv& recvPkg) {
    for(int i = 0; i < sizeof(recvPkg); i++)
    {
        std::cout << std::hex << (int)((char*)&recvPkg)[i] << " ";
    }
    std::cout << std::endl;
}

void printBytes(PingPkg& pkg) {
    for(int i = 0; i < sizeof(pkg); i++)
    {
        std::cout << std::hex << (int)((char*)&pkg)[i] << " ";
    }
    std::cout << std::endl;
}

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
    pkg.hdr.type = ICMP_ECHO;
    pkg.hdr.un.echo.id = getpid() % std::numeric_limits<u_int16_t>::max();

    // pkg.setNbAsMsg(msg);

    // pkg.hdr.un.echo.sequence = this->seqCounter.nb;
    // this->seqCounter++;
    // pkg.hdr.checksum = checksum(&pkg, sizeof(pkg));

    PingPkgRecv recvPkg;
    bzero(&recvPkg, sizeof(recvPkg));

    int fromlen, selectRes;

    std::string currIP;
    timeval timeout {0, 10000};
    // std::set<uint32_t> toSend, toRead;


    // 4 sock read & write
    fd_set toRead, toSend;
    int maxSockNb = -1;
    FD_ZERO(&toRead);
    FD_ZERO(&toSend);
    for(std::map<std::uint32_t, PingConfig>::iterator it = this->addrCfgs.begin(); it != this->addrCfgs.end(); ++it) {
        currIP = it->second.IP;
        if (addrToSock[currIP] > maxSockNb) {
            maxSockNb = addrToSock[currIP];
        }

        if (it->second.status == PingStatus::W_4_ANSV) {
            FD_SET(addrToSock[currIP], &toRead);
        }
        else if (it->second.status == PingStatus::W_4_SEND) {
            FD_SET(addrToSock[currIP], &toSend);
        }
    }

    // select block
    selectRes = select(maxSockNb+1, &toRead, &toSend, NULL, &timeout);

    if (selectRes == 0) {
        std::cout << "timeout\n";
        for(std::map<std::uint32_t, PingConfig>::iterator it = this->addrCfgs.begin(); it != this->addrCfgs.end(); ++it) {
            currIP = it->second.IP;
            if (it->second.status == PingStatus::W_4_ANSV) {
                it->second.status = PingStatus::W_4_SEND;
                FD_SET(addrToSock[currIP], &toSend);
            }
            // else if (it->second.status == PingStatus::ERR) {
            //     FD_SET(addrToSock[currIP], &toSend);
            // }
        }
    }

    for(std::map<std::uint32_t, PingConfig>::iterator it = this->addrCfgs.begin(); it != this->addrCfgs.end(); ++it) {
        currIP = it->second.IP;
        std::cout << "curr IP: " << currIP << std::endl;

        // read
        if (FD_ISSET(addrToSock[currIP], &toRead)) {
            bzero(&recvPkg, sizeof(recvPkg));
            fromlen = sizeof(sockaddr_in);

            // std::cout << "recv: " << addrToSock[currIP] << "|";
            // printBytes(recvPkg);
            // std::cout << "|" << inet_ntoa(addrToSockAddr[currIP].sin_addr) << "|" << fromlen << std::endl;
            // TODO: resolve
            
            if (recvfrom(
                    addrToSock[currIP], 
                    &recvPkg, 
                    sizeof(recvPkg), 
                    0,
                    (struct sockaddr*)&(addrToSockAddr[currIP]), 
                    (socklen_t*)&fromlen
                    ) <= 0) {
                        it->second.status = PingStatus::ERR;
                        std::cout << "Packet receive failed!" << std::endl;
                    }
            else {
                printBytes(recvPkg);
                std::cout << "ansv: " << *((uint32_t*) recvPkg.ping_pkg.msg) << std::endl;
                this->addrCfgs[*((uint32_t*) recvPkg.ping_pkg.msg)].status = PingStatus::OK;
            }
            // std::cout << "recv aft: " << addrToSock[currIP] << "|";
            // printBytes(recvPkg);
            // std::cout << "|" << inet_ntoa(addrToSockAddr[currIP].sin_addr) << "|" << fromlen << std::endl;
        }

        // write
        else if (FD_ISSET(addrToSock[currIP], &toSend)) {
            std::cout << "asd\n";

            std::cout << "nb: " << it->first << std::endl;
            pkg.setNbAsMsg(it->first);

            pkg.hdr.un.echo.sequence = this->seqCounter.nb;
            this->seqCounter++;

            pkg.hdr.checksum = checksum(&pkg, sizeof(pkg));

            std::cout << "sending: ";
            printBytes(pkg);

            if (sendto(
                    addrToSock[currIP], 
                    &pkg, 
                    sizeof(pkg), 
                    0, 
                    (struct sockaddr*)&(this->addrToSockAddr[currIP]), 
                    (socklen_t)sizeof(sockaddr_in)
                    ) <= 0
                ) {
                    it->second.status = PingStatus::ERR;
                    printf("\nPacket Sending Failed!\n");
                }
            else {
                it->second.status = PingStatus::W_4_ANSV;
            }
        }
    }
    // if (flag) {


    // bzero(&recvPkg, sizeof(recvPkg));
    // fromlen = sizeof(sockaddr_in);
    
    // currIP = "173.194.222.101";
    //             if (recvfrom(
    //                 addrToSock[currIP], 
    //                 &recvPkg, 
    //                 sizeof(recvPkg), 
    //                 0,
    //                 (struct sockaddr*)&(addrToSockAddr[currIP]), 
    //                 (socklen_t*)&fromlen
    //                 ) <= 0) {
    //                     std::cout << "Packet receive failed!" << std::endl;
    //                 }
    //         else {
    //             printBytes(recvPkg);
    //             std::cout << "ansv: " << *((uint32_t*) recvPkg.ping_pkg.msg) << std::endl;
    //             this->addrCfgs[*((uint32_t*) recvPkg.ping_pkg.msg)].status = PingStatus::OK;
    //         }


    // }


    // }
    return this->addrCfgs;
}

Ping::~Ping() 
{

}
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
    timeval timeout {1, 0};
    // std::set<uint32_t> toSend, toRead;

    // 4 sock read & write
    // fd_set toRead, toSend;
    // int maxSockNb = -1;
    // FD_ZERO(&toRead);
    // FD_ZERO(&toSend);
    // for(std::map<std::uint32_t, PingConfig>::iterator it = this->addrCfgs.begin(); it != this->addrCfgs.end(); ++it) {
    //     currIP = it->second.IP;
    //     if (addrToSock[currIP] > maxSockNb) {
    //         maxSockNb = addrToSock[currIP];
    //     }

    //     if (it->second.status == PingStatus::W_4_ANSV) {
    //         FD_SET(addrToSock[currIP], &toRead);
    //     }
    //     else if (it->second.status == PingStatus::W_4_SEND) {
    //         FD_SET(addrToSock[currIP], &toSend);
    //     }
    // }

    // // select block
    // selectRes = select(maxSockNb+1, &toRead, &toSend, NULL, &timeout);

    // for(std::map<std::uint32_t, PingConfig>::iterator it = this->addrCfgs.begin(); it != this->addrCfgs.end(); ++it) {
    //     currIP = it->second.IP;
    //     std::cout << "curr IP: " << currIP << std::endl;

    //     // read
    //     if (FD_ISSET(addrToSock[currIP], &toRead)) {
    //         bzero(&recvPkg, sizeof(recvPkg));
    //         fromlen = sizeof(sockaddr_in);

    //         // std::cout << "recv: " << addrToSock[currIP] << "|";
    //         // printBytes(recvPkg);
    //         // std::cout << "|" << inet_ntoa(addrToSockAddr[currIP].sin_addr) << "|" << fromlen << std::endl;
    //         // TODO: resolve
    //         if (recvfrom(
    //                 addrToSock[currIP], 
    //                 &recvPkg, 
    //                 sizeof(recvPkg), 
    //                 0,
    //                 (struct sockaddr*)&(addrToSockAddr[currIP]), 
    //                 (socklen_t*)&fromlen
    //                 ) <= 0) {
    //                     it->second.status = PingStatus::ERR;
    //                     std::cout << "Packet receive failed!" << std::endl;
    //                 }
    //         else {
    //             printBytes(recvPkg);
    //             std::cout << "ansv: " << *((uint32_t*) recvPkg.ping_pkg.msg) << std::endl;
    //             this->addrCfgs[*((uint32_t*) recvPkg.ping_pkg.msg)].status = PingStatus::OK;
    //         }
    //         // std::cout << "recv aft: " << addrToSock[currIP] << "|";
    //         // printBytes(recvPkg);
    //         // std::cout << "|" << inet_ntoa(addrToSockAddr[currIP].sin_addr) << "|" << fromlen << std::endl;
    //     }

    //     // write
    //     else if (FD_ISSET(addrToSock[currIP], &toSend)) {
    //         std::cout << "asd\n";

    //         std::cout << "nb: " << it->first << std::endl;
    //         pkg.setNbAsMsg(it->first);

    //         pkg.hdr.un.echo.sequence = this->seqCounter.nb;
    //         this->seqCounter++;

    //         pkg.hdr.checksum = checksum(&pkg, sizeof(pkg));

    //         if (sendto(
    //                 addrToSock[currIP], 
    //                 &pkg, 
    //                 sizeof(pkg), 
    //                 0, 
    //                 (struct sockaddr*)&(this->addrToSockAddr[currIP]), 
    //                 (socklen_t)sizeof(sockaddr_in)
    //                 ) <= 0
    //             ) {
    //                 it->second.status = PingStatus::ERR;
    //                 printf("\nPacket Sending Failed!\n");
    //             }
    //         else {
    //             it->second.status = PingStatus::W_4_ANSV;
    //         }
    //     }
    // }


    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);


                pkg.setNbAsMsg(3);

            pkg.hdr.un.echo.sequence = this->seqCounter.nb;
            this->seqCounter++;

            pkg.hdr.checksum = checksum(&pkg, sizeof(pkg));

    if (sendto(
                    sock, 
                    &pkg, 
                    sizeof(pkg), 
                    0, 
                    (struct sockaddr*)&(this->addrToSockAddr["173.194.222.101"]), 
                    (socklen_t)sizeof(sockaddr_in)
                    ) <= 0
                ) {
                    // it->second.status = PingStatus::ERR;
                    printf("\nPacket Sending Failed!\n");
                }
            else {
                std::cout << "sended" << 3 << std::endl;
                // it->second.status = PingStatus::W_4_ANSV;
            }


                pkg.setNbAsMsg(2);

            pkg.hdr.un.echo.sequence = this->seqCounter.nb;
            this->seqCounter++;

            pkg.hdr.checksum = checksum(&pkg, sizeof(pkg));

    if (sendto(
                    sock, 
                    &pkg, 
                    sizeof(pkg), 
                    0, 
                    (struct sockaddr*)&(this->addrToSockAddr["5.255.255.80"]), 
                    (socklen_t)sizeof(sockaddr_in)
                    ) <= 0
                ) {
                    // it->second.status = PingStatus::ERR;
                    printf("\nPacket Sending Failed!\n");
                }
            else {
                std::cout << "sended" << 2 << std::endl;
                // it->second.status = PingStatus::W_4_ANSV;
            }

            msghdr msghdr;
            char payload_buffer[4096];
            ssize_t payload_buffer_len;
            char control_buffer[4096];
            struct iovec payload_iovec;

            while (1) {
                fd_set toRead;
                FD_ZERO(&toRead);

                FD_SET(sock, &toRead);

                select(10, &toRead, NULL, NULL, &timeout);

                if (FD_ISSET(sock, &toRead)) {
                    memset (&payload_iovec, 0, sizeof (payload_iovec));
                    payload_iovec.iov_base = payload_buffer;
                    payload_iovec.iov_len = sizeof (payload_buffer);

                    memset (&msghdr, 0, sizeof (msghdr));
                    msghdr.msg_name = NULL;
                    msghdr.msg_namelen = 0;

                    msghdr.msg_iov = &payload_iovec;
                    msghdr.msg_iovlen = 1;

                    msghdr.msg_control = control_buffer;
                    msghdr.msg_controllen = sizeof (control_buffer);

                    msghdr.msg_flags = 0;

                    payload_buffer_len = recvmsg (sock, &msghdr, /* flags = */ 0);

                    std::cout << "len" << payload_buffer_len << std::endl;
                    for(int i = 0; i < payload_buffer_len; i++)
                    {
                        std::cout << std::hex << (int)(payload_buffer)[i] << " ";
                    }
                    std::cout << std::endl;
                }

            }

            // memset (&payload_iovec, 0, sizeof (payload_iovec));
            // payload_iovec.iov_base = payload_buffer;
	        // payload_iovec.iov_len = sizeof (payload_buffer);

            // memset (&msghdr, 0, sizeof (msghdr));
            // msghdr.msg_name = NULL;
            // msghdr.msg_namelen = 0;

            // msghdr.msg_iov = &payload_iovec;
            // msghdr.msg_iovlen = 1;

            // msghdr.msg_control = control_buffer;
	        // msghdr.msg_controllen = sizeof (control_buffer);

            // msghdr.msg_flags = 0;

            // payload_buffer_len = recvmsg (sock, &msghdr, /* flags = */ 0);

            // std::cout << "len" << payload_buffer_len << std::endl;
            // for(int i = 0; i < payload_buffer_len; i++)
            // {
            //     std::cout << std::hex << (int)(payload_buffer)[i] << " ";
            // }
            // std::cout << std::endl;

            //     if (recvmsg(
            //         sock, 
            //         &recvPkg, 
            //         sizeof(recvPkg), 
            //         0,
            //         (struct sockaddr*)&(addrToSockAddr[currIP]), 
            //         (socklen_t*)&fromlen
            //         ) <= 0) {
            //             it->second.status = PingStatus::ERR;
            //             std::cout << "Packet receive failed!" << std::endl;
            //         }
            // else {
            //     printBytes(recvPkg);
            //     std::cout << "ansv: " << *((uint32_t*) recvPkg.ping_pkg.msg) << std::endl;
            //     this->addrCfgs[*((uint32_t*) recvPkg.ping_pkg.msg)].status = PingStatus::OK;
            // }

    // }

    // debug vars
    // uint32_t msg = 1;
    // debug vars end

    // for(std::map<std::uint32_t, PingConfig>::iterator it = this->addrCfgs.begin(); it != this->addrCfgs.end(); ++it) {
    //     toSend.insert(it->first);
    // }

    // std::set<uint32_t>::iterator er;
    // // toSend.erase(er);

    // fd_set sockets_fds;

    // std::string currIP;
    // while (!toSend.empty()) {
    //     FD_ZERO(&sockets_fds);

    //     for (std::set<uint32_t>::iterator it = toSend.begin(); it != toSend.end(); it++) {
    //         currIP = this->addrCfgs[*it].IP;
    //         std::cout << "ip adding: " << currIP << std::endl; 
    //         FD_SET(addrToSock[currIP], &sockets_fds);
    //     }

    //     // on send 






    //     er = toSend.begin();
    //     std::cout << "rm: " << *er << std::endl;
    //     toSend.erase(er);
    // }

    // pkg.hdr.type = ICMP_ECHO;
    // // std::cout << "pid: " << getpid() << " " << std::endl;
    // pkg.hdr.un.echo.id = getpid() % std::numeric_limits<u_int16_t>::max();
    // std::cout << "pid echo: " << pkg.hdr.un.echo.id << " " << std::endl;
    // pkg.setNbAsMsg(msg);

    // pkg.hdr.un.echo.sequence = this->seqCounter.nb;
    // this->seqCounter++;
    // pkg.hdr.checksum = checksum(&pkg, sizeof(pkg));

    // sockaddr_in s_addr = this->addrToSockAddr["173.194.222.101"];
    // int s = this->addrToSock["173.194.222.101"];

    // if ( 
    //     sendto(
    //         s, 
    //         &pkg, 
    //         sizeof(pkg), 
    //         0, 
    //         (struct sockaddr*) 
    //         &s_addr, 
    //         (socklen_t)sizeof(sockaddr_in)
    //         ) <= 0
    //     ) {
    //         printf("\nPacket Sending Failed!\n");
    //     }

    // fromlen = sizeof(sockaddr_in);
    // if ( 
    //     recvfrom(
    //         s, 
    //         &recvPkg, 
    //         sizeof(recvPkg), 
    //         0,
    //         (struct sockaddr*)&s_addr, 
    //         (socklen_t*)&fromlen
    //         ) <= 0
    //     ) {
    //         printf("\nPacket receive failed!\n");
    //     }

    // std::cout << *((uint32_t*) recvPkg.ping_pkg.msg) << std::endl;

    return this->addrCfgs;
}

Ping::~Ping() 
{

}
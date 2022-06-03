#ifndef PING_H
#define PING_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
// #include <netinet/ip.h>
// #include <netinet/in_systm.h>
#include <string>
#include <map>
#include <limits>

#define PING_PKT_S 64

struct Counter 
{
    u_int16_t nb;

    Counter& operator++(int)
    {
        if (this->nb == std::numeric_limits<u_int16_t>::max()) {
            this->nb = 0;
        }
        else {
            this->nb++;
        }
        return *this;
    }
};


struct PingPkg
{
    struct icmphdr hdr;
    char msg[PING_PKT_S-sizeof(icmphdr)];

    void setNbAsMsg(uint32_t nb)
    {
        bzero(&(this->msg), sizeof(this->msg));
        uint32_t* uint32_msg = (uint32_t*)(this->msg);
        *uint32_msg = nb;
        return;
    }
};

unsigned short checksum(void *b, int len);

struct PingConfig {
    std::string IP;
//    std::uint32_t port;
    std::string name;
    std::uint32_t Id;
    int status;
};

class Ping
{
private:
    std::map<std::uint32_t, PingConfig> addrCfgs;
    std::map<std::string, sockaddr_in> addrToSockAddr;
    std::map<std::string, int> addrToSock;
    //PingPkg pingPkg;


public:
    Ping(std::map<std::uint32_t, PingConfig> addrCfgs);
    bool Init();
    std::map<std::uint32_t, PingConfig> Exec();
    ~Ping();
};

#endif // PING_H

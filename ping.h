#ifndef PING_H
#define PING_H

// #include "icmp_msgs.h"
#include <arpa/inet.h>
#include <cstring>
#include <limits>
#include <map>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <set>
#include <stdlib.h>
#include <string>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define PING_PKT_S 64

static timeval timeout{0, 10000};
#define TIMEOUT timeout

struct SeqCounter {
    u_int16_t nb = 0;

    SeqCounter &operator++(int) {
        if (this->nb == std::numeric_limits<u_int16_t>::max()) {
            this->nb = 0;
        } else {
            this->nb++;
        }
        return *this;
    }
};

#pragma pack(push, 1)
struct PingPkg {
    struct icmphdr hdr;
    char msg[PING_PKT_S - sizeof(icmphdr)];

    void setNbAsMsg(uint32_t nb) {
        bzero(&(this->msg), sizeof(this->msg));
        uint32_t *uint32_msg = (uint32_t *)(this->msg);
        *uint32_msg = nb;
        return;
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PingPkgRecv {
    struct ip ip_hdr;
    struct PingPkg ping_pkg;
};
#pragma pack(pop)

unsigned short checksum(void *b, int len);

enum class PingStatus { W_4_SEND, W_4_ANSV, OK, ERR };

struct PingConfig {
    std::string IP;
    //    std::uint32_t port;
    std::string name;
    std::uint32_t Id;
    PingStatus status = PingStatus::W_4_SEND;
    // IcmpMsg icmpMsg;
};

class Ping {
  private:
    std::map<std::uint32_t, PingConfig> addrCfgs;
    std::map<std::string, sockaddr_in> addrToSockAddr;
    int sock;
    SeqCounter seqCounter;

  public:
    Ping(std::map<std::uint32_t, PingConfig> addrCfgs);
    bool Init();
    std::map<std::uint32_t, PingConfig> Exec();
    ~Ping();
};

#endif // PING_H

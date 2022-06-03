#ifndef PING_H
#define PING_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <map>


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

public:
    Ping(std::map<std::uint32_t, PingConfig> addrCfgs);
    bool Init();
    std::map<std::uint32_t, PingConfig> Exec();
    ~Ping();
};

#endif // PING_H

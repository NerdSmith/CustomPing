#include "ping.h"

int ping_receive_one(int sock, std::map<std::uint32_t, PingConfig> &addrCfgs) {
    struct msghdr msg_hdr;
    char payload_buffer[4096];
    ssize_t payload_buffer_len;
    char control_buffer[4096];
    struct iovec payload_iovec;
    icmphdr *hdr;

    int data_offset;
    uint32_t msg_key;

    memset(&payload_iovec, 0, sizeof(payload_iovec));
    payload_iovec.iov_base = payload_buffer;
    payload_iovec.iov_len = sizeof(payload_buffer);

    memset(&msg_hdr, 0, sizeof(msg_hdr));

    msg_hdr.msg_name = NULL;
    msg_hdr.msg_namelen = 0;

    msg_hdr.msg_iov = &payload_iovec;
    msg_hdr.msg_iovlen = 1;

    msg_hdr.msg_control = control_buffer;
    msg_hdr.msg_controllen = sizeof(control_buffer);

    msg_hdr.msg_flags = 0;

    payload_buffer_len = recvmsg(sock, &msg_hdr, 0);

    if (payload_buffer_len < 0) {
        return -1;
    }

    data_offset = payload_buffer_len - PING_PKT_S + sizeof(icmphdr);

    msg_key = *((uint32_t *)(payload_buffer + (payload_buffer_len - PING_PKT_S +
                                               sizeof(icmphdr))));
    if (addrCfgs.find(msg_key) != addrCfgs.end()) {
        addrCfgs[msg_key].status = PingStatus::OK;
    } else {
        return -1;
    }

    return 0;
}

int ping_send_one(int sock, u_int16_t nb, uint32_t msg_key,
                  std::map<std::uint32_t, PingConfig> &addrCfgs,
                  std::map<std::string, sockaddr_in> &addrToSockAddr) {
    ssize_t send_size;
    std::string currIP = addrCfgs[msg_key].IP;

    PingPkg pkg;
    memset(&pkg, 0, sizeof(pkg));

    pkg.hdr.type = ICMP_ECHO;
    pkg.hdr.un.echo.id = getpid() % std::numeric_limits<u_int16_t>::max();
    pkg.setNbAsMsg(msg_key);

    pkg.hdr.un.echo.sequence = nb;

    pkg.hdr.checksum = checksum(&pkg, sizeof(pkg));

    send_size = sendto(sock, &pkg, sizeof(pkg), 0,
                       (struct sockaddr *)&(addrToSockAddr[currIP]),
                       (socklen_t)sizeof(sockaddr_in));
    if (send_size <= 0) {
        addrCfgs[msg_key].status = PingStatus::ERR;
        return 1;
    } else {
        addrCfgs[msg_key].status = PingStatus::W_4_ANSV;
    }

    return 0;
}

unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

Ping::Ping(std::map<std::uint32_t, PingConfig> addrCfgs) {
    this->addrCfgs = addrCfgs;
}

bool Ping::Init() {
    for (const auto &kv : this->addrCfgs) {

        addrToSockAddr[kv.second.IP];
        addrToSockAddr[kv.second.IP].sin_family = AF_INET;
        addrToSockAddr[kv.second.IP].sin_addr.s_addr =
            inet_addr(kv.second.IP.c_str());
    }
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        return false;
    }

    return true;
}

std::map<std::uint32_t, PingConfig> Ping::Exec() {
    int max_fd = sock;
    uint32_t usecTimeout = (uint32_t)(
        (TIMEOUT.tv_sec * 1000000 + TIMEOUT.tv_usec) / this->addrCfgs.size());

    for (std::map<std::uint32_t, PingConfig>::iterator it =
             this->addrCfgs.begin();
         it != this->addrCfgs.end(); ++it) {

        fd_set read_fds;
        fd_set write_fds;

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        timeval currTimeout{usecTimeout / 1000000, usecTimeout % 1000000};

        if (it->second.status == PingStatus::W_4_ANSV) {
            FD_SET(sock, &read_fds);
        }

        if (it->second.status == PingStatus::W_4_SEND) {
            FD_SET(sock, &write_fds);
        }

        int status =
            select(max_fd + 1, &read_fds, &write_fds, NULL, &currTimeout);

        if (status == -1) {
            return this->addrCfgs;
        } else if (status == 0) {
            continue;
        }

        if (FD_ISSET(sock, &read_fds)) {
            ping_receive_one(this->sock, this->addrCfgs);
            continue;
        }

        if (FD_ISSET(sock, &write_fds)) {
            ping_send_one(this->sock, this->seqCounter.nb, it->first,
                          this->addrCfgs, this->addrToSockAddr);
            this->seqCounter.nb++;
        }
    }

    return this->addrCfgs;
}

Ping::~Ping() {}
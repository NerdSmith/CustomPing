//#include <QCoreApplication>
#include <iostream>
#include <netinet/ip.h>
#include <stdio.h>

#include "ping.h"

void printPingConfig(const PingConfig &pCfg) {
    std::string status;
    switch (pCfg.status) {
    case (PingStatus::W_4_SEND):
        status = "W_4_send";
        break;
    case (PingStatus::W_4_ANSV):
        status = "W_4_ANSV";
        break;
    case (PingStatus::OK):
        status = "OK";
        break;
    }

    std::cout << pCfg.IP << " " << pCfg.name << " " << pCfg.Id << " " << status
              << "\n";
}

void printMap(std::map<std::uint32_t, PingConfig> myMap) {
    for (std::map<std::uint32_t, PingConfig>::const_iterator it = myMap.begin();
         it != myMap.end(); ++it) {
        std::cout << it->first << " : ";
        printPingConfig(it->second);
    }
}

bool checkAllStatus(std::map<std::uint32_t, PingConfig> addrss) {
    for (std::map<std::uint32_t, PingConfig>::iterator it = addrss.begin();
         it != addrss.end(); ++it) {
        if (it->second.status != PingStatus::OK) {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {

    PingConfig pc1;
    pc1.IP = "173.194.222.101";
    pc1.name = "google";
    pc1.Id = 1;

    PingConfig pc2;
    pc2.IP = "5.255.255.80";
    pc2.name = "yandex";
    pc2.Id = 2;

    PingConfig pc3;
    pc3.IP = "31.13.72.36";
    pc3.name = "facebook";
    pc3.Id = 3;

    std::map<std::uint32_t, PingConfig> testAddrss = {
        {3, pc1},
        {2, pc2},
        {4, pc3},
    };
    printMap(testAddrss);
    Ping ping(testAddrss);
    std::cout << "init: " << ping.Init() << std::endl;

    int c;
    while (!checkAllStatus(testAddrss) && c < 15) {
        testAddrss = ping.Exec();
        printMap(testAddrss);
        c++;
    }

    // sleep(1);

    // sleep(1);

    return 0;
}

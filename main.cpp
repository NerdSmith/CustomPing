//#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include <netinet/ip.h>

#include "ping.h"


void printPingConfig(const PingConfig& pCfg) 
{
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

    std::cout << pCfg.IP << " " << pCfg.name << " " << pCfg.Id << " " << status << "\n";
}

void printMap(std::map<std::uint32_t, PingConfig> myMap) 
{
    for(std::map<std::uint32_t, PingConfig>::const_iterator it = myMap.begin(); it != myMap.end(); ++it)
    {
        std::cout << it->first << " : ";
        printPingConfig(it->second);
    }
}


int main(int argc, char *argv[])
{
    
    std::map<std::uint32_t, PingConfig> testAddrss = {
        {3, {"173.194.222.101", "google", 1}},
        {2, {"5.255.255.80", "yandex", 2}},
        {4, {"31.13.72.36", "facebook", 3}}
    };
    printMap(testAddrss);
    Ping ping(testAddrss);
    std::cout << "init: " << ping.Init() << std::endl;
    testAddrss = ping.Exec();
    printMap(testAddrss);
    // sleep(1);
    testAddrss = ping.Exec();
    printMap(testAddrss);
    // sleep(1);
    testAddrss = ping.Exec();
    printMap(testAddrss);

    testAddrss = ping.Exec();
    printMap(testAddrss);

    testAddrss = ping.Exec();
    printMap(testAddrss);
    // sleep(1);
    testAddrss = ping.Exec();
    printMap(testAddrss);
    // sleep(1);
    testAddrss = ping.Exec();
    printMap(testAddrss);
    
    testAddrss = ping.Exec();
    printMap(testAddrss);

    testAddrss = ping.Exec();
    printMap(testAddrss);
    // sleep(1);
    testAddrss = ping.Exec();
    printMap(testAddrss);

    testAddrss = ping.Exec();
    printMap(testAddrss);

    testAddrss = ping.Exec();
    printMap(testAddrss);
    // sleep(1);
    testAddrss = ping.Exec();
    printMap(testAddrss);
    // sleep(1);
    testAddrss = ping.Exec();
    printMap(testAddrss);
    
    testAddrss = ping.Exec();
    printMap(testAddrss);

    return 0;
}

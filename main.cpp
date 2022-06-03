//#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include <netinet/ip.h>

#include "ping.h"

void printPingConfig(const PingConfig& pCfg) 
{
    std::cout << pCfg.IP << " " << pCfg.name << " " << pCfg.Id << " " << pCfg.status << "\n";
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
        {1, {"173.194.222.101", "google", 1, 0}},
        {2, {"5.255.255.80", "yandex", 2, 0}}
    };
    printMap(testAddrss);
    Ping ping(testAddrss);
    ping.Init();


    return 0;
}

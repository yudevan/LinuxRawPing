
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <string>
#include <cstring>

#include "socket.h"
#include "icmp.h"

using namespace std;

int main(int argc, char** argv) 
{
    uint32_t DESTINATION = IPv4::stringIpToInt("192.168.1.1");
    uint32_t SOURCE = IPv4::stringIpToInt("192.168.1.7");
    
    int TOTAL_SENT = 5;

    // Create a Ping Packet
    IPv4::ICMP ping(SOURCE, DESTINATION);
    
    // Create the Socket
    try
    {
        IPv4::Socket socket(IPv4::SEND);

        for(int i = 0; i < TOTAL_SENT; i++)
        {
            ping.timeStamp();
            ping.calcCheckSums();
            ping.printVerboseHeaderPacket();
            
            std::cout << std::endl;
            
            socket.sendTo(ping);
        }
    }
    catch(ErrorEx ex)
    {
        std::cerr << std::endl << ex.what() << std::endl;
        exit(-1);
    }
    
    return EXIT_SUCCESS;
}


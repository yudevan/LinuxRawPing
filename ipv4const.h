#pragma once

#include <string>
#include <vector>
#include <inttypes.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "ipconst.h"

namespace IPv4
{
    enum DIFF_SERVICES_CODE_POINT { DIFF_DEFAULT = 0 
                                  }; 
    
    enum EXPLICIT_CONGESTION {  NON_ECN_CAPABLE = 0, 
                                ECN_CAPABLE_0 = 2, 
                                ECN_CAPABLE_1 = 1, 
                                CONGESTION_ENCOUNTERED = 3 
                            }; 
    
    const uint16_t IPv4_DEAFULT_HEADER_LENGTH = 20;
    
    const uint16_t MIN_PACKET_LEN = 20;
    
    const uint16_t MAX_PACKET_LEN = 65535;
    
    const uint16_t IDENTIFICATION = 0;
    
    enum FLAGS {    FLAGS_DEFAULT = 0, 
                    DONT_FRAGMENT = 2, 
                    MORE_FRAGMENTS = 1
                };
    
    const uint16_t NO_FRAG_OFFSET = 0;
    
    const uint8_t DEFAULT_TTL = 64;
    
    const int ICMP_HEADER_LENGTH = 8;
    
    const int BLOCK_SIZE = 1024;
    
    void stringIpToVector(std::string strIP, std::vector<uint8_t> &byteVector);
       
    uint32_t stringIpToInt(std::string strIP);
       
    bool isValidIP(uint32_t ip);
    
    uint32_t randomIP();
    
    // A class to assist with Addresses    
    class SocketAddress
    {
     public:
        struct sockaddr_in  ip;
        
     public:
        // RAW ICMP
        SocketAddress()
        {
            init (htonl(INADDR_ANY), htons(0) );
        }
        
        // Simple bind
        SocketAddress(uint16_t port)
        {
            init (htonl(INADDR_ANY), htons(port) );
        }
        
        // Used for ping
        SocketAddress(std::string strIP )
        {
            uint32_t temp = 0;
            inet_pton(AF_INET, strIP.c_str(), &temp);
            init( temp, 0 );
        }
        
        // TCP, UDP, etc (layer 4)
        SocketAddress(uint32_t addrIP, uint16_t port)
        {
            init( htonl(addrIP), htons(port) );
        }
        
        // TCP, UDP, etc (layer 4)
        SocketAddress(std::string strIP, uint16_t port)
        {
            uint32_t temp = 0;
            
            inet_pton(AF_INET, strIP.c_str(), &temp);
            init( temp, htons(port));
        }
        
        virtual ~SocketAddress()
        { 
        }
        
    protected:
        void init(uint32_t address, uint16_t port)
        {
            memset( (void*) &ip, 0, sizeof(struct sockaddr_in)); 
            
            
            ip.sin_family = AF_INET;
            ip.sin_port = port;
            ip.sin_addr.s_addr = address;
        } 
    };
}


#pragma once

#include <netinet/in.h>

namespace IP_CONST
{
    enum IP_VERSION { IP_V4 = 4, 
                      IP_V6 = 6
                    };
    
    enum SOCKET_FAMILY {    IPV4 = AF_INET, 
                            IPV6 = AF_INET6
                       };
    
    enum SOCKET_TYPE {  SOCK_TYPE_IP = IPPROTO_IP, 
                        SOCK_TYPE_TCP = SOCK_STREAM, 
                        SOCK_TYPE_UDP = SOCK_DGRAM, 
                        SOCK_TYPE_RAW = SOCK_RAW
                     };
    
    enum SOCKET_PROTOCOL {  PROTOCOL_ICMP = IPPROTO_ICMP, 
                            PROTOCOL_IP = IPPROTO_IP, 
                            PROTOCOL_RAW = IPPROTO_RAW, 
                            PROTOCOL_TCP = IPPROTO_TCP, 
                            PROTOCOL_UDP = IPPROTO_UDP
                         };
    
    enum SOCKET_DIRECTION { SEND, 
                            RECEIVE 
                          };
}



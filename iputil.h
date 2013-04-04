#pragma once

#include <iostream>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>  
#include <inttypes.h>
#include "ipconst.h"

namespace IP_UTILITY 
{
    void getIPType(uint8_t *pBytes, IP_CONST::IP_VERSION &ver, IP_CONST::SOCKET_PROTOCOL &type);
    
    void randomVector(std::vector<uint8_t> &vec, int size);
   
    uint16_t reverseEndian16(uint16_t data);
    
    uint32_t reverseEndian32(uint32_t data);
    
    uint64_t reverseEndian64(uint64_t data);
    
    int is_big_endian();
}


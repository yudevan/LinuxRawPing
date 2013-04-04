#include "iputil.h"


void IP_UTILITY::getIPType(uint8_t *pBytes, IP_CONST::IP_VERSION &ver, IP_CONST::SOCKET_PROTOCOL &type)
{
    ver = static_cast<IP_CONST::IP_VERSION>( static_cast<int>(pBytes[0]) );
    
    switch(ver)
    {
        case IP_CONST::IP_V4:
            type = static_cast<IP_CONST::SOCKET_PROTOCOL>( static_cast<int>( pBytes[9] ) );
            break;
        case IP_CONST::IP_V6:
            type = static_cast<IP_CONST::SOCKET_PROTOCOL>( static_cast<int>( pBytes[6] ) );
            break;
    }
    
}

void IP_UTILITY::randomVector(std::vector<uint8_t> &vec, int size)
{
    for(int i = 0; i < size; i++)
    {
        vec.push_back(rand() % 256);
    }
}

uint16_t IP_UTILITY::reverseEndian16(uint16_t data)
{
    return ( ((data >> 8) & 0x00FF) | ((data << 8) & 0xFF00) );
}

uint32_t IP_UTILITY::reverseEndian32(uint32_t data)
{
    return ( ((data >> 24) & 0x000000FF) | ((data >>  8) & 0x0000FF00) | ((data <<  8) & 0x00FF0000) | ((data << 24) & 0xFF000000) );
}

uint64_t IP_UTILITY::reverseEndian64(uint64_t data)
{
    return
    ( ((data >> 56) & 0x00000000000000FF) | ((data >> 40) & 0x000000000000FF00) | ((data >> 24) & 0x0000000000FF0000) | ((data >>  8) & 0x00000000FF000000) | ((data <<  8) & 0x000000FF00000000) | ((data << 24) & 0x0000FF0000000000) | ((data << 40) & 0x00FF000000000000) | ((data << 56) & 0xFF00000000000000) );
}

int IP_UTILITY::is_big_endian()
{
    union 
    {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};
    
    return bint.c[0] == 1; 
}

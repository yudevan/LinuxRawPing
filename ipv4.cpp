#include "ipv4.h"

void IPv4::stringIpToVector(std::string strIP, std::vector<uint8_t> &byteVector)
{
    int byte;
    std::string temp;
    size_t pos = strIP.find(".", 0); 
    byteVector.clear();
    
    while (pos != std::string::npos)
    {
        temp = strIP.substr(0, pos);
        strIP.erase(0, pos + 1);
        std::stringstream num(temp);
        num >> byte;
        byteVector.push_back((uint8_t) byte);
        
        pos = strIP.find(".", 0);
    }
    
    std::stringstream num(strIP);
    num >> byte;
    byteVector.push_back((uint8_t) byte);
}

uint32_t IPv4::stringIpToInt(std::string strIP)
{
    uint32_t ret = 0;
    int byte;
    std::string temp;
    size_t pos = strIP.find(".", 0); 
    
    while (pos != std::string::npos)
    {
        temp = strIP.substr(0, pos);
        strIP.erase(0, pos + 1);
        std::stringstream num(temp);
        num >> byte;
        ret = (ret << 8) | (uint8_t)byte;
        
        pos = strIP.find(".", 0);
    }
    
    std::stringstream num(strIP);
    num >> byte;
    ret = (ret << 8) | (uint8_t)byte;
    
    return ret;
}

bool IPv4::isValidIP(uint32_t ip)
{
    return true;
}

uint32_t IPv4::randomIP()
{
    uint32_t ip = 0;
    
    do{
        ip = ((rand() % 254) + 1 );
        ip = ip << 8;
        ip = ip | (rand() % 256);
        ip = ip << 8;
        ip = ip | (rand() % 256);
        ip = ip << 8;
        ip = ip | ((rand() % 254) + 1);
    }while(isValidIP(ip) != true);
    
    return ip;
}



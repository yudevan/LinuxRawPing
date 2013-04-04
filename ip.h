#pragma once

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <ctime>

#include <string>
#include <vector>
#include <sstream>

#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h> 
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <inttypes.h>

#include <stdexcept>

#include "mutex.h"

class IPbuffer : public std::vector<uint8_t>
{
 protected: 
    Mutex mtx;
    
 public:
    IPbuffer()
    {
    } 
    
    IPbuffer(const IPbuffer &copy)
    {
        set(copy);
    }
    
    virtual ~IPbuffer()
    {
    }

 public:
    virtual void append(const std::vector<uint8_t> &vec)
    {
        std::size_t vecLen = vec.size();
        
        for(int i = 0; i < vecLen; i++)
        {
            push_back(vec[i]);
        }
    }
    
    virtual void append(const uint8_t *vec, int size)
    {
        for(int i = 0; i < size; i++)
        {
            push_back(vec[i]);
        }
    }
    
    virtual void set(const std::vector<uint8_t> &vec)
    {
        (std::vector<uint8_t>)*this = vec;
    }
    
    virtual void set(const uint8_t *vec, int size)
    {
        clear();
        
        for(int i = 0; i < size; i++)
        {
                this->push_back(vec[i]);
        }
    }
    
};

class ErrorEx : public std::runtime_error
{
 protected:
    int err;
    
 public:
    ErrorEx(std::exception ex) : std::runtime_error( ex.what() )
    {
    }
    
    ErrorEx(int e) : std::runtime_error(strerror(e))
    {
    }

    ErrorEx(const std::string &errString) : std::runtime_error(errString), err(0)
    {
    }
    
    ErrorEx( const std::string &errString, int e ) : std::runtime_error(std::string(errString + strerror(e)) ), err(e)
    {
    }
    
    virtual ~ErrorEx() throw()
    {   
    }
};






    
    


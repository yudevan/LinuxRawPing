#pragma once

#include "ipv4.h"

namespace IPv4
{

using namespace IP_CONST;
     
// IPv4 Socket
class Socket
{
    
 protected:
    
    int socketDescriptor;       // File Descriptor
    bool socketClosed;          // If socket is open or closed
    SOCKET_TYPE type;           // Protocol for socket
    
    // IPv4 address + port
    struct sockaddr_in  ip;     // Address
    
    // Constants for socket
    static const int DEFAULT_PACKET_SIZE = 2048;
    static const int DEFAULT_OPTIONAL_ADDRESS = 128;
    static const int MAX_CONNECT_NODES = 5;
    
    // Storage buffers
    uint8_t controlBuffer[DEFAULT_PACKET_SIZE];
    uint8_t optionalAddress[DEFAULT_OPTIONAL_ADDRESS];
    uint8_t packet[DEFAULT_PACKET_SIZE];
    
    // Data storage area structures for I/O ( using uio )
    struct iovec ioVector;
    struct msghdr msg;
    
 protected:
    
    void init()
    {
        memset(&msg, 0, sizeof(msg));
        
        ioVector.iov_base  = packet;
        ioVector.iov_len = DEFAULT_PACKET_SIZE;
        msg.msg_name = optionalAddress;
        msg.msg_namelen = DEFAULT_OPTIONAL_ADDRESS;
        msg.msg_iov  = &ioVector;
        msg.msg_iovlen  = 1;
        msg.msg_control = controlBuffer;
        msg.msg_controllen = DEFAULT_PACKET_SIZE;
    }
    
 public:
    
    // Most users require a simple setup based on the protocol (even raw sockets)
    Socket(SOCKET_DIRECTION direction, const SocketAddress &address = SocketAddress()) 
    {    
        // Save the address
        ip = address.ip;
        
        // Used for some socket Options
        int value = 1;
        
        init();
         
        socketDescriptor = socket(IPV4, SOCK_TYPE_RAW, PROTOCOL_ICMP);
        isError(socketDescriptor);
        
        switch(direction)
        {
            case SEND:
                    isError(setsockopt(socketDescriptor, PROTOCOL_IP, IP_HDRINCL, &value, sizeof(int)));
                    break;
            case RECEIVE:
                    isError(bind(socketDescriptor, (struct sockaddr *) &ip, sizeof(ip)));
                    break;
                            
        }
        
        socketClosed = false;
        
    }
        
    ~Socket()
    {
        closeSocket();
    }
    
    void closeSocket()
    {
        close(socketDescriptor);
        
        socketClosed = true;
    }
    
    void setIP(SocketAddress &address)
    {
        ip = address.ip;
    }
    
    void bindToSocket()
    {
        isError(bind(socketDescriptor, (struct sockaddr *) &ip, sizeof(ip)));
        
        // Start to listen. Hold a max of 5 connecting nodes in the wait queue
        isError(listen(socketDescriptor, MAX_CONNECT_NODES));
    }
    
    void sendTo(IPv4::IP &packet)
    {
        // Send the packet
        struct sockaddr_in sa = packet.getSockAddr();
        isError(sendto(socketDescriptor, &packet[0], packet.size(), 0, (struct sockaddr *)&sa, sizeof(struct sockaddr_in) ) );
    }
    
    int receiveMessage(IPbuffer &buffer)
    {   
        int bytesReceived = 0;
        bytesReceived = recvmsg(socketDescriptor, &msg, 0);
        isError(bytesReceived);
        
        if(bytesReceived > 0)
        {
            buffer.set( (uint8_t *)msg.msg_iov->iov_base, bytesReceived);    
        }
        return bytesReceived;
    }
    
    void readBytes(IPbuffer &buffer)
    {
        int num_read = 0;
        uint8_t block[BLOCK_SIZE];
        bool done = false;
        
        while (done == false)
        {
            num_read = read(socketDescriptor, (char *)block, BLOCK_SIZE);
            
            socketErrRead(num_read, block);
            
            if (num_read == 0)
            {
                done = true;
            }
            else if(num_read > 0)
            {
                buffer.append(block, num_read);
            }
        }
        
    }
    
    void sendBytes(IPbuffer &buffer)
    {
        uint8_t* pBuffer = &buffer[0];
        int num_left = buffer.size();
        int num_written = 0;
        
        while (num_left > 0)
        {
            num_written = write(socketDescriptor, (char*)pBuffer, num_left);
            
            socketErrWrite(num_written, pBuffer);
            
            num_left -= num_written;
            pBuffer += num_written;
        }
        
    }
    
 protected:
    
    void isError(int returnCode)
    {
        if(returnCode < 0)
        {
            throw ErrorEx(errno);
        }
    }
    
    void socketErrRead(int errCode, uint8_t* pBuffer)
    {
        if(errCode < 0 && errno == EINTR )
        {
            while(errCode < 0 && errno == EINTR)
            {
                errCode = read(socketDescriptor, (char*)pBuffer, BLOCK_SIZE);
            }
        }
        
        if(errCode < 0 && errno != EINTR)
        {
            throw ErrorEx(errno);
        }
    }
    
    void socketErrWrite(int errCode, uint8_t* pBuffer)
    {
        if(errCode < 0 && errno == EINTR )
        {
            while(errCode < 0 && errno == EINTR)
            {
                errCode = write(socketDescriptor, (char*)pBuffer, BLOCK_SIZE);
            }
        }
        
        if(errCode < 0 )
        {
            throw ErrorEx(errno);
        }
    }
  
};

}



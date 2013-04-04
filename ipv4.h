#pragma once

#include "ipv4const.h"
#include "ip.h"

namespace IPv4 
{
    
    class IP : public IPbuffer
    {
        
     public:
        IP()
        {
            std::vector<uint8_t> empty;
            createDefaultPacket(empty);
        }
        
        IP(const IP &model)
        {
            set( model );
        }
        
        IP(std::vector<uint8_t> &data)
        {
            createDefaultPacket(data);
        }
        
        virtual ~IP()
        {
        }

    public:
        
        virtual uint16_t createDefaultPacket(std::vector<uint8_t> &data)
        {
            clear();
            
            std::size_t vecLen = data.size();
            reserve(IPv4_DEAFULT_HEADER_LENGTH + vecLen );
            
            setVersion(IP_CONST::IP_V4);
            setInternetHeaderLength(IPv4_DEAFULT_HEADER_LENGTH / 4);
            setTotalLength(IPv4_DEAFULT_HEADER_LENGTH + vecLen);
            setDiffServices(DIFF_DEFAULT);
            setExplicitCongestion(NON_ECN_CAPABLE);
            setIdentification(IDENTIFICATION);
            setFlags(FLAGS_DEFAULT);
            setFragmentOffset(NO_FRAG_OFFSET);
            setTimeToLive(DEFAULT_TTL);
                
            calcChecksum();

            std::size_t offset = IPv4_DEAFULT_HEADER_LENGTH;
            
            for(std::size_t i = 0; i < vecLen; i++, offset++)
            {
                at(offset) = data[i];
            }
                
            return getTotalLength();
            
        }
        
    public:  
        
        void setVersion(uint8_t ver)
        {
            at(0) = (at(0) & 15) | (ver << 4);
        }
        
        uint8_t getVersion()
        {
            return (at(0) >> 4);
        }
        
        void setInternetHeaderLength(uint8_t len)
        {
            at(0) = (at(0) & 240) | len;
        }
        
        uint8_t getInternetHeaderLength()
        {
            return (at(0) & 15);
        }
        
        uint16_t getHeaderLength()
        {
            return (at(0) & 15) * 4;
        }
        
        void setDiffServices(uint8_t diff)
        {
            at(1) = (at(1) & 3) | (diff << 2);
        }
        
        uint8_t getDiffServices()
        {
            return (at(1) >> 2);
        }
        
        void setExplicitCongestion(uint8_t congestion)
        {
            at(1) = (at(1) & 252) | congestion;
        }
        
        uint8_t getExplicitCongestion()
        {
            return (at(1) & 3);
        }
        
        void setTotalLength(uint16_t len)
        {
            at(2) = (uint8_t) len >> 8;
            at(3) = (uint8_t) len;
        }
        
        uint16_t getTotalLength()
        {
            uint16_t ret = 0;
            ret = at(2);
            
            ret = ret << 8;
            ret = ret | at(3);
            
            return ret;
        }
        
        void setIdentification(uint16_t id)
        {
            at(4) = (uint8_t) id >> 8;
            at(5) = (uint8_t) id;
            
        }
        
        uint16_t getIdentification()
        {
            uint16_t ret = 0;
            
            ret = at(4);
            ret = ret << 8;
            ret = ret | at(5);
            
            return ret;
        }
        
        void setFlags(uint8_t flags )
        {
            flags = flags << 5;
            
            at(6) = (at(6) & 31) | flags;
        }
        
        uint8_t getFlags()
        {
            return ( (at(6) & 224) >> 5);
        }
        
        void setFragmentOffset(uint16_t frag)
        {
            at(6) = (at(6) & 224) | (frag >> 8);
            at(7) = (at(7) & 255) | ((uint8_t) frag);
        }
        
        uint16_t getFragmentOffset()
        {
            uint16_t ret = 0;
            
            ret = (at(6) & 31);
            ret = ret << 8;
            ret = ret | at(7);
            
            return ret;
        }
        
        void setTimeToLive(uint8_t ttl)
        {
            at(8) = ttl;
        }
        
        uint8_t getTimeToLive()
        {
            return at(8);
        }
        
        void setProtocol(uint8_t proto)
        {
            at(9) = proto;
        }
        
        uint8_t getProtocol()
        {
            return at(9);
        }
        
    public:     
        
        void setHeaderChecksum(uint16_t c)
        {
            at(10) = (uint8_t) c >> 8;
            at(11) = (uint8_t) c;
        }
        
        uint16_t getHeaderChecksum()
        {
            uint16_t ret = 0;
            
            ret = at(10);
            ret = ret << 8;
            ret = ret | at(11);
            
            return ret;
        }

        void calcChecksum()
        {
            /*  The 16 bit one's complement of the one's complement sum of all 16
                bit words in the header.  For computing the checksum, the checksum
                field should be zero.  This checksum may be replaced in the
                future.
             */
            int headerEnd = getHeaderLength() / 2;
     
            const int16_t *csPacket = (int16_t *) &( (*this)[0]);
            int32_t sum = 0;
            
            setHeaderChecksum(0);

            for(int i = 0; i < headerEnd; i++)
            {
                sum = sum + ntohs(csPacket[i]);
            }  
       
            sum = ~sum;

            setHeaderChecksum(sum);
        }
        
        virtual void calcCheckSums()
        {
            calcChecksum();
        }
        
        void setOptions(std::vector<uint8_t> &op)
        {
            uint8_t size = op.size();
            uint8_t offset = IPv4_DEAFULT_HEADER_LENGTH;
            std::vector<uint8_t> tempPacket;
            
            tempPacket.reserve( size + offset + getDataLength() );
            uint8_t i = 0;
            
            for(; i < offset; i++)
            {
                tempPacket[i] = (*this)[i];
            }

            for(uint8_t j = 0; i < size; i++, j++)
            {
                tempPacket[i] = op[j];
            }

            for(uint16_t k = getHeaderLength(); k < getTotalLength(); i++, k++)
            {
                tempPacket[i] = (*this)[k];
            }
            
            clear();
            *this = tempPacket;
            setInternetHeaderLength( (size + offset) / 4);
            
        }
        
        void getOptions(std::vector<uint8_t> &op)
        {
            uint8_t offset = IPv4_DEAFULT_HEADER_LENGTH;
            uint16_t size = getHeaderLength() - offset;
            
            for(uint8_t i = 0; i < size; i++)
            {
                op.push_back( at(offset + i));
            }
        }
  
    public:
        void setSourceAddress(std::vector<uint8_t> &ip)
        {
            if(ip.size() == 4)
            {
                at(12) = ip[0];
                at(13) = ip[1];
                at(14) = ip[2];
                at(15) = ip[3];
            }
        }
        
        void setSourceAddress(uint32_t ip)
        {
            at(12) = (uint8_t)(ip >> 24);
            at(13) = (uint8_t)(ip >> 16);
            at(14) = (uint8_t)(ip >> 8);
            at(15) = (uint8_t)(ip);
        }
        
        void getSourceAddress(std::vector<uint8_t> &ip)
        {
            ip.push_back(at(12) );
            ip.push_back(at(13) );
            ip.push_back(at(14) );
            ip.push_back(at(15) );
        }
        
        uint32_t getSourceAddress()
        {
            uint32_t ret = 0;
            
            ret = at(12);
            ret = ret << 8;
            ret = ret | at(13);
            ret = ret << 8;
            ret = ret | at(14);
            ret = ret << 8;
            ret = ret | at(15);
            
            return ret;
        }
        
        void setDestinationAddress(std::vector<uint8_t> &ip)
        {
            if(ip.size() == 4)
            {
                at(16) = ip[0];
                at(17) = ip[1];
                at(18) = ip[2];
                at(19) = ip[3];
            }
        }
        
        void setDestinationAddress(uint32_t ip)
        {
            at(16) = (uint8_t)(ip >> 24);
            at(17) = (uint8_t)(ip >> 16);
            at(18) = (uint8_t)(ip >> 8);
            at(19) = (uint8_t)(ip);
        }
        
        void getDestinationAddress(std::vector<uint8_t> &ip)
        {
            ip.clear();
            
            ip.push_back(at(16));
            ip.push_back(at(17));
            ip.push_back(at(18));
            ip.push_back(at(19));
        }
        
        uint32_t getDestinationAddress()
        {
            uint32_t ret = 0;
            
            ret = at(16);
            ret = ret << 8;
            ret = ret | at(17);
            ret = ret << 8;
            ret = ret | at(18);
            ret = ret << 8;
            ret = ret | at(19);
            
            return ret;
        }
        
        virtual sockaddr_in getSockAddr()
        {
            struct sockaddr_in address;
            memset(&address, 0, sizeof(sockaddr_in)); 
            
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = getDestinationAddress();
            
            return address;
        }

    public:
        virtual void getHeader(std::vector<uint8_t> &vec)
        {
            uint16_t header = getHeaderLength();
            
            for(uint8_t i = 0; i < header; i++)
            {
                vec.push_back((*this)[i]);
            }
        }
        
        virtual void getData(std::vector<uint8_t> &vec)
        {
            uint16_t offset = getHeaderLength();
            uint16_t totSize = getTotalLength();
            
            for(uint16_t i = offset; i < totSize; i++)
            {
                vec.push_back((*this)[i]);
            }
        }
        
        virtual uint16_t getDataLength()
        {
            uint16_t offset = getHeaderLength();
            uint16_t totSize = getTotalLength();
            
            return (totSize - offset);
        }
        
        void getPacket(std::vector<uint8_t> &vec)
        {
            int totSize = getTotalLength();
            
            for(int i = 0; i < totSize; i++)
            {
                vec.push_back((*this)[i]);
            }
        }
        
    public:
        virtual void printRawPacket()
        {
            printf("\n Beginning Packet\n");
            
            uint16_t i = 0;
            uint16_t j = 0;
            uint16_t totSize = getTotalLength();
            
            for(; i < totSize; i++, j++)
            {
                if(j == 4)
                {
                    j = 0;
                    printf("\n");                       
                }
                printf("%.2X ", (*this)[i]);
                
            }
            
            printf("\nEnd Packet\n");
        }
        
        virtual void printRawPayload()
        {
            printf("\n Beginning Payload\n");
            
            uint8_t i = getHeaderLength();
            uint8_t j = 0;
            uint16_t totSize = ntohs(getTotalLength());
            
            for(; i < totSize; i++, j++)
            {
                if(j == 4)
                {
                    j = 0;
                    printf("\n");                       
                }
                printf("%.2X ", (*this)[i]);
                
            }
            
            printf("\nEnd Payload\n");
        }
        
        virtual void printVerboseHeaderPacket()
        {
            std::vector<uint8_t> ip;
            
            printf(" IP Version: %d\n", getVersion());
            printf(" IP InternetHeaderLength: %d\n", getInternetHeaderLength());
            printf(" IP DiffServices: %d\n", getDiffServices());
            printf(" IP ExplicitCongestion: %d\n", getExplicitCongestion());
            printf(" IP TotalLength: %d\n", getTotalLength());
            printf(" IP Identification: %d\n", getIdentification());
            printf(" IP Flags: %d\n", getFlags());
            printf(" IP FragmentOffset: %d\n", getFragmentOffset());
            printf(" IP TimeToLive: %d\n", getTimeToLive());
            printf(" IP Protocol: %d\n", getProtocol());
            printf(" IP Header Checksum: %d\n", getHeaderChecksum());
            printf(" IP Source: ");
            getSourceAddress(ip);
            printf("%.3d.%.3d.%.3d.%.3d\n", ip[0],ip[1],ip[2],ip[3]);
            ip.clear();
            printf(" IP Destination: ");
            getDestinationAddress(ip);
            printf("%.3d.%.3d.%.3d.%.3d\n", ip[0],ip[1],ip[2],ip[3]);
        }
    };
}



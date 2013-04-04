#pragma once

#include "ipv4.h"

namespace IPv4
{
    
    enum TYPE { ECHO_REPLY = 0, 
                DESTINATION_UNREACHABLE = 3, 
                SOURCE_QUENCH = 4, 
                REDIRECT_MESSAGE = 5, 
                ECHO_REQUEST = 8, 
                ROUTER_ADVERTISMENT = 9, 
                ROUTER_SOLICITATION = 10, 
                TIME_EXCEEDED = 11, 
                BAD_IP_HEADER = 12, 
                TIMESTAMP = 13, 
                TIMESTAMP_REPLY = 14, 
                INFORMATION_REQUEST = 15, 
                INFORMATION_REPLY = 16, 
                ADDRESS_MASK_REQUEST = 17, 
                ADDRESS_MASK_REPLY = 18, 
                TRACEROUTE = 30 
             };
             
    namespace echo_reply
    {
        enum CODE_ECHO_REPLY { CODE = 0 };
    }
    
    namespace unreachable
    {
    enum CODE_UNREACHABLE { DESTINATION_NETWORK_UNREACHABLE = 0, 
                            DESTINATION_HOST_UNREACHABLE, 
                            DESTINATION_PROTOCOL_UNREACHABLE, 
                            DESTINATION_PORT_UNREACHABLE, 
                            FRAGMENTATION_REQUIRED, 
                            SOURCE_ROUTE_FAILED, 
                            DESTINATION_NETWORK_UNKNOWN, 
                            DESTINATION_HOST_UNKNOWN, 
                            SOURCE_HOST_ISOLATED, 
                            NETWORK_ADMINISTRATIVELY_PROHIBITED, 
                            HOST_ADMINISTRATIVELY_PROHIBITED, 
                            NETWORK_UNREACHABLE_FOR_TOS, 
                            HOST_UNREACHABLE_FOR_TOS, 
                            COMMUNICATION_ADMINISTRATIVELY_PROHIBITED, 
                            HOST_PRECEDENCE_VIOLATION, 
                            PRECEDENCE_CUTOFF_IN_EFFECT 
                         };
    }
    
    namespace echo_request
    {
        enum CODE_ECHO_REQUEST { CODE = 0 };
    }
    
    const int ECHO_REQUEST_HEADER_SIZE = 16;
    enum ECHO_REQUEST_DEFAULTS { IDENTIFIER = 0,
                                 SEQUENCE_NUMBER = 0
                               };

    class ICMP : public IP
    {

    public:
        
        ICMP()
        {
            std::vector<uint8_t> timestamp;
            timestamp.reserve(sizeof(time_t) );
            
            for(std::size_t i = 0; i < timestamp.size(); i++)
            {
                timestamp[i] = 0;
            }
            
            createDefaultPacket(timestamp);
        }
        
        ICMP(uint32_t src, uint32_t dst, TYPE type = ECHO_REQUEST, int code = 0, const std::vector<uint8_t> &data = std::vector<uint8_t>())
        {
            createDefaultPacket(src, dst, type, code, data);     
        }
        
        virtual ~ICMP()
        {
        }
        
    public:        
        
        virtual uint16_t createDefaultPacket(const std::vector<uint8_t> &data)
        {
            clear();
            
            std::size_t vecLen = data.size();
            std::size_t offset = IPv4_DEAFULT_HEADER_LENGTH + ECHO_REQUEST_HEADER_SIZE;
            reserve(offset + vecLen );
            
            setVersion(IP_CONST::IP_V4);
            setInternetHeaderLength(IPv4_DEAFULT_HEADER_LENGTH / 4);
            setTotalLength(IPv4_DEAFULT_HEADER_LENGTH + vecLen);
            setDiffServices(DIFF_DEFAULT);
            setExplicitCongestion(NON_ECN_CAPABLE);
            setIdentification(IDENTIFICATION);
            setFlags(DONT_FRAGMENT);
            setFragmentOffset(NO_FRAG_OFFSET);
            setTimeToLive(DEFAULT_TTL);
            
            setProtocol(IP_CONST::PROTOCOL_ICMP);
            setType(ECHO_REQUEST);
            setCode(echo_request::CODE);

            calcCheckSums();
            
            for(std::size_t i = 0; i < vecLen; i++, offset++)
            {
                (*this)[offset] = data[i];
            }
            
            return getTotalLength();
        }
        
        virtual uint16_t createDefaultPacket(uint32_t src, uint32_t dst, TYPE type, int code, const std::vector<uint8_t> &data)
        {
            uint16_t ret = 0;
            clear();
            switch(type)
            {
                case ECHO_REQUEST:
                    ret = createEchoRequest(data);
                    break;
                default:
                    // Error
                    break;
            }
            
            setType(type);
            setCode(code);
            setSourceAddress(src);
            setDestinationAddress(dst);
        
            calcCheckSums();
            
            return ret;
        }
        
    public:

        uint16_t createEchoRequest(const std::vector<uint8_t> &data)
        {
            // [ Id | Sequence Number ]
            // [     Time Stamp       ]
            std::size_t vecLen = data.size();
            std::size_t offset = IPv4_DEAFULT_HEADER_LENGTH + ECHO_REQUEST_HEADER_SIZE;
            reserve(offset + vecLen );
            
            int j = offset;
            
            for(std::size_t i = 0; i < vecLen; i++,j++)
            {
                (*this)[j] = data[i];
            }
            
            setVersion(IP_CONST::IP_V4);
            setInternetHeaderLength(IPv4_DEAFULT_HEADER_LENGTH / 4);
            setTotalLength(offset + vecLen);
            setDiffServices(DIFF_DEFAULT);
            setExplicitCongestion(NON_ECN_CAPABLE);
            setIdentification(IDENTIFICATION);
            setFlags(DONT_FRAGMENT);
            setFragmentOffset(NO_FRAG_OFFSET);
            setTimeToLive(DEFAULT_TTL);
            
            setProtocol(IP_CONST::PROTOCOL_ICMP);
            setIdentifier(IDENTIFIER);
            setSequenceNumber(SEQUENCE_NUMBER);
            
            return getTotalLength();
        }

    public:
        void setType(uint8_t t)
        {
            uint8_t byte  =  t;
            int offset = getHeaderLength();
         
            at(offset) = byte;
                       
        }
        
        uint8_t getType()
        {
            int offset = getHeaderLength();
            return at(offset);
            
        }
        
        void setCode(uint8_t c)
        {
            uint8_t byte  = c;
            int offset = getHeaderLength();
            
            at(offset + 1) = byte;
            
        }
        
        uint8_t getCode()
        {
            int offset = getHeaderLength();
            
            return at(offset + 1);
        }
        
        void setICMPChecksum(uint16_t c)
        {
            int offset = getHeaderLength();
            
            at(offset + 2) = (uint8_t)(c >> 8);
            at(offset + 3) = (uint8_t) c;
        }
        
        uint16_t getICMPChecksum()
        {
            uint16_t ret = 0;
            int offset = getHeaderLength();
            
            ret = at(offset + 2);
            ret = (ret << 8) | at(offset + 3);
            
            return ret;
        }

        void calcICMPChecksum()
        {
            /*
               The checksum is the 16-bit ones's complement of the one's
               complement sum of the ICMP message starting with the ICMP Type.
               For computing the checksum , the checksum field should be zero.
               If the total length is odd, the received data is padded with one
               octet of zeros for computing the checksum.  This checksum may be
               replaced in the future.
             */
            
            int icmpBegin = getHeaderLength()/2;
            int icmpEnd = getTotalLength()/2;
           
            const int16_t *csPacket = (int16_t *) &((*this)[0]);
            int32_t sum = 0;
            
            setICMPChecksum(0);

            for(int i = icmpBegin; i < icmpEnd; i++)
            {
                sum = sum + ntohs(csPacket[i]);
            }  
          
            sum = ~sum;
            
            setICMPChecksum(sum);
        }
        
        virtual void calcCheckSums()
        {
            IP::calcChecksum();
            calcICMPChecksum();
        }
        
        void setRestOfHeader(uint32_t options)
        {
            int offset = getHeaderLength();
            
            at(offset + 4) = (uint8_t)(options >> 24);
            at(offset + 5) = (uint8_t)(options >> 16);
            at(offset + 6) = (uint8_t)(options >> 8);
            at(offset + 7) = (uint8_t) options;
        }
        
        uint32_t getRestOfHeader()
        {
            uint32_t ret = 0;
            int offset = getHeaderLength();
            
            ret = at(offset + 4);
            ret = (ret << 8) | at(offset + 5);
            ret = (ret << 8) | at(offset + 6);
            ret = (ret << 8) | at(offset + 7);
            
            return ret;
            
        }
        
        void setIdentifier(uint16_t ID)
        {
            int offset = getHeaderLength();
            
            at(offset + 4) = (uint8_t)(ID >> 8);
            at(offset + 5) = (uint8_t) ID;
        }
        
        uint16_t getIdentifier()
        {
            uint16_t ret = 0;
            int offset = getHeaderLength();
            
            ret = at(offset + 4);
            ret = (ret << 8) | at(offset + 5);
            
            return ret;
        }
        
        void setSequenceNumber(uint16_t sequence)
        {
            int offset = getHeaderLength();
            
            at(offset + 6) = (uint8_t)(sequence >> 8);
            at(offset + 7) = (uint8_t) sequence;
        }
        
        uint16_t getSequenceNumber()
        {
            uint16_t ret = 0;
            int offset = getHeaderLength();
            
            ret = at(offset + 6);
            ret = (ret << 8) | at(offset + 7);
            
            return ret;
            
        }
        
        void setTimeStamp(uint64_t ts)
        {
            int offset = getHeaderLength();
            
            at(offset + 8) = (uint8_t) (ts >> 56);
            at(offset + 9) = (uint8_t) (ts >> 48);
            at(offset + 10) = (uint8_t) (ts >> 40);
            at(offset + 11) = (uint8_t) (ts >> 32);
            
            at(offset + 12) = (uint8_t) (ts >> 24);
            at(offset + 13) = (uint8_t) (ts >> 16);
            at(offset + 14) = (uint8_t) (ts >> 8);
            at(offset + 15) = (uint8_t) ts;
        }
        
        uint64_t getTimeStamp()
        {
            uint64_t ret = 0;
            int offset = getHeaderLength();
            
            ret = at(offset + 8);
            ret = (ret << 8) | at(offset + 9);
            ret = (ret << 8) | at(offset + 10);
            ret = (ret << 8) | at(offset + 11);
            
            ret = (ret << 8) | at(offset + 12);
            ret = (ret << 8) | at(offset + 13);
            ret = (ret << 8) | at(offset + 14);
            ret = (ret << 8) | at(offset + 15);
            
            return ret;
        }
        
        void timeStamp()
        {
            setTimeStamp((uint64_t) time(NULL));
        }
        
        virtual void getHeader(std::vector<uint8_t> &vec)
        {
            uint16_t beginHeader = getHeaderLength();
            uint16_t endHeader = beginHeader;
            uint16_t type = getType();
    
            endHeader += getHeaderOffset();
            
            for(uint16_t i = beginHeader; i < endHeader ; i++)
            {
                vec.push_back((*this)[i]);
            }
        }
        
        virtual void getData(std::vector<uint8_t> &vec)
        {
            uint16_t offset = getHeaderLength() + getHeaderOffset();
            uint16_t totSize = getTotalLength();
            
            for(uint16_t i = offset; i < totSize; i++)
            {
                vec.push_back((*this)[i]);
            }
        }
        
        uint16_t getHeaderOffset()
        {
            uint16_t type = getType();
            
            switch(type) 
            {
                case ECHO_REQUEST:
                    return ECHO_REQUEST_HEADER_SIZE;
                    break;
                default:
                    break;
            }    
        }
        
        virtual void printVerboseHeaderPacket()
        {
            TYPE icmpType = (TYPE) getType();
            
            IP::printVerboseHeaderPacket();
            
            switch(icmpType)
            {
                case ECHO_REQUEST:
                    printf(" ICMP Type: %d\n", getType());
                    printf(" ICMP Code: %d\n", getCode());
                    printf(" ICMP Checksum: %d\n", getICMPChecksum());
                    printf(" ICMP Identifier: %d\n", getIdentifier());
                    printf(" ICMP Sequence Number: %d\n", getSequenceNumber());
                    printf(" ICMP Time Stamp: %llu\n", getTimeStamp());
                    break;
                default:
                    break;
            }
        }
    };
    
}


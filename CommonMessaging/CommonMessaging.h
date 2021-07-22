#ifndef _COMMON_MESSAGING_H_
#define _COMMON_MESSAGING_H_

#include <Arduino.h>

/*
   Protocol
   --------

   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   Start Code  |         Source Address        |Destination Ad.|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |               |         Payload Length        | System Command|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   Start Code:  8 bits
     Used to indicate start of message

   Source Address:  16 bits
     The source address

   Destination Address: 16 bits
     The destination address

   Payload Length: 16 bits
     Size of the apyload after the header in bytes

   System Command: 8 bits
     Specifies network commands to perform
*/

#define SOFT_BAUD 9600

#define PING_BYTE byte(0xAA)
#define ACK_BYTE byte(0xAB)
#define START_CODE byte(0xAC)
#define LISTEN_WAIT 5

// Hardware serial has a special case address
#define PORT_H 0xffff
#define EMPTY 0x0000

// System commands
#define ROUTER_GET_ID 0x01
#define ROUTER_GET_NEIGHBORS 0x02
#define ROUTER_ADD_NODE 0x04
#define ROUTER_START_DISCOVERY 0x08
#define ROUTER_GET_DISCOVERY_STATUS 0x10
#define ROUTER_RESPONSE_DISCOVERY_STATUS 0x20
#define ROUTER_CLEAR_TOPOLOGY 0x40
#define ROUTER_SYS_COMMAND 0x80
#define ROUTER_USE_CACHE 0x100

typedef uint8_t StartCode_t;
typedef uint16_t NodeId_t;
typedef uint16_t MessageSize_t;
typedef uint16_t SysCommand_t;

class Message
{
public:
    Message(NodeId_t source, NodeId_t dest, MessageSize_t payloadSize, SysCommand_t sysCommand, byte *body) : source(source), dest(dest), payloadSize(payloadSize), sysCommand(sysCommand), body(body) {}
    Message() {}

    NodeId_t getSource() { return source; }
    void setSource(NodeId_t source) { this->source = source; }
    
    NodeId_t getDest() { return dest; }
    void setDest(NodeId_t dest) { this->dest = dest; }
    
    MessageSize_t getPayloadSize() { return payloadSize; }
    
    SysCommand_t getSysCommand() { return sysCommand; }
    void setSysCommand(SysCommand_t sysCommand) { this->sysCommand = sysCommand; }
    
    byte *getBody() { return body; }
    void setBody(byte *body) { this->body = body; }

    void free();

private:
    NodeId_t source = EMPTY;
    NodeId_t dest = EMPTY;
    MessageSize_t payloadSize = 0;
    SysCommand_t sysCommand = 0;
    byte *body = NULL;
};

bool ackWait(Stream *port, int maxRetries = -1);

bool hasIncoming(Stream *port);

Message readMessage(Stream *srcPort);

void writeMessage(Stream *destPort, const Message &message);

#endif // _COMMON_MESSAGING_H_

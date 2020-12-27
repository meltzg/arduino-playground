#ifndef _COMMON_MESSAGING_H_
#define _COMMON_MESSAGING_H_

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

#define PING_BYTE byte(0xAA)
#define ACK_BYTE byte(0xAB)
#define START_CODE byte(0xAC)
#define PING_DELAY 1
#define LISTEN_WAIT 2

// System commands
#define ROUTER_GET_ID 0x01
#define ROUTER_GET_NEIGHBORS 0x02
#define ROUTER_CLEAR_TOPOLOGY 0x20
#define ROUTER_ADD_NODE 0x04
#define ROUTER_START_DISCOVERY 0x08
#define ROUTER_GET_DISCOVERY_STATUS 0x10

typedef uint8_t StartCode_t;
typedef uint16_t NodeId_t;
typedef uint16_t MessageSize_t;
typedef uint8_t SysCommand_t;

struct Message {
  NodeId_t source;
  NodeId_t dest;
  MessageSize_t payloadSize;
  SysCommand_t sysCommand;
  byte *body;
};

bool ackWait(Stream *port, int maxRetries = -1);

bool hasIncoming(Stream *port);

Message readMessage(Stream *srcPort);

void writeMessage(Stream *destPort, const Message &message);

#endif _COMMON_MESSAGING_H_

#include <SoftwareSerial.h>

#define PING_BYTE byte(0xAA)
#define ACK_BYTE byte(0xAB)
#define START_CODE byte(0xAC)
#define READ_DELAY 50
#define PING_DELAY 1
#define LISTEN_WAIT 2

// Hardware serial has a special case address
#define PORT_H 0xffffffff
#define EMPTY 0x00000000
// System commands
#define GET_ID 0x01
#define UPDATE_NEIGHBORS 0x02
#define GET_NEIGHBORS 0x04
#define GET_TOPOLOGY 0x08
#define UPDATE_TOPOLOGY 0x10
#define DISCOVER_TOPOLOGY 0x20

typedef uint8_t StartCode_t;
typedef uint32_t NodeId_t;
typedef uint16_t MessageSize_t;
typedef uint8_t SysCommand_t;

/*
   Protocol
   --------

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   Start Code  |                 Source Address                |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |               |              Destination Address              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |               |         Payload Length        | System Command|
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   Start Code:  8 bits
     Used to indicate start of message

   Source Address:  32 bits
     The source address

   Destination Address: 32 bits
     The destination address

   Payload Length: 16 bits
     Size of the apyload after the header in bytes

   System Command: 8 bits
     Specifies network commands to perform

   System Commands
   ---------------
   Get ID: 0x01
     Retrieve the ID of the node. During newtork discovery, this can be used
     to determine the ID of one's neightbors

   Update Neighbors: 0x02
     Payload contains the ordered neighbor IDs of the source node

   Get Neighbors: 0x04
     Retrieve the neighbors of the Source address

   Update Network Topology: 0x08
     The payload contains updated network topology to use for routing

   Get Network Topology: 0x10
     Return the node's network topology

   Discover: 0x20
     Discover network topology, create and distribute routing tables
*/

/*
   Each node has a SoftwareSerial connection to its neighbor and another to the
   1 /\ 2
   0 | | 3
   5 \/ 4

   The 7th port (PORT_ACTOR) sends/receves messages through the network
*/
#if defined(__AVR_ATmega328P__)
#define STATUS_LED A2
SoftwareSerial PORT_0(2, 3);
SoftwareSerial PORT_1(4, 5);
SoftwareSerial PORT_2(6, 7);
SoftwareSerial PORT_3(8, 9);
SoftwareSerial PORT_4(10, 11);
SoftwareSerial PORT_5(12, 13);
SoftwareSerial PORT_ACTOR(14, 15);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define STATUS_LED A0
SoftwareSerial PORT_0(10, 11);
SoftwareSerial PORT_1(12, 13);
SoftwareSerial PORT_2(50, 51);
SoftwareSerial PORT_3(52, 53);
SoftwareSerial PORT_4(62, 63);
SoftwareSerial PORT_5(65, 66);
SoftwareSerial PORT_ACTOR(67, 68);
#endif

SoftwareSerial *NEIGHBORS[6] = {&PORT_0, &PORT_1, &PORT_2, &PORT_3, &PORT_4, &PORT_5};
SoftwareSerial *ALLPORTS[7] = {&PORT_0, &PORT_1, &PORT_2, &PORT_3, &PORT_4, &PORT_5, &PORT_ACTOR};
NodeId_t neighborIds[6] = { EMPTY };

NodeId_t NODE_ID;

void setup() {
  NODE_ID = getNodeId();
  Serial.begin(9600);
  PORT_0.begin(9600);
  PORT_1.begin(9600);
  PORT_2.begin(9600);
  PORT_3.begin(9600);
  PORT_4.begin(9600);
  PORT_5.begin(9600);
  PORT_ACTOR.begin(9600);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);
  delay(1000);
  digitalWrite(STATUS_LED, LOW);
}

void loop() {
  NodeId_t source, dest;
  MessageSize_t payloadSize;
  SysCommand_t sysCommand;
  byte *body;
  // Loop through ports and process messages
  if (Serial.available() > 0) {
    readMessage(&Serial, source, dest, payloadSize, sysCommand, body);
    source = PORT_H;
    processMessage(&Serial, source, dest, payloadSize, sysCommand, body);
    delete[] body;
    body = NULL;
  }

  for (int i = 0; i < 7; i++) {
    SoftwareSerial *port = ALLPORTS[i];
    port->listen();
    if (hasIncoming(port)) {
      Serial.println("recieving message");
      readMessage(port, source, dest, payloadSize, sysCommand, body);
      Serial.write((char *)body, payloadSize);
      Serial.println();
      processMessage(port, source, dest, payloadSize, sysCommand, body);
      delete[] body;
      body = NULL;
    }
  }
}

bool ackWait(Stream *port, int maxRetries = -1) {
  bool connected = false;
  char ackBuff[1];
  for (int i = 0; !connected && (i < maxRetries || maxRetries < 0); i++) {
    Serial.println("pinging");
    port->write(PING_BYTE);
    byte pong = port->read();
    Serial.println(pong, HEX);
    if (pong == ACK_BYTE) {
      Serial.println("Connected!");
      return true;
    }
    delay(PING_DELAY);
  }
  Serial.println("Failed!");
  return false;
}

bool hasIncoming(Stream *port) {
  delay(LISTEN_WAIT);
  byte resp = port->read();
  if (resp == PING_BYTE) {
    Serial.println("incoming data");
    port->write(ACK_BYTE);
    return true;
  }
  return false;
}

void readMessage(Stream *srcPort, NodeId_t &source, NodeId_t &dest, MessageSize_t &payloadSize, SysCommand_t &sysCommand, byte *&body) {
  byte startByte;
  do {
    srcPort->readBytes(&startByte, sizeof(StartCode_t));
  } while (startByte != START_CODE);
  srcPort->readBytes((byte *) &source, sizeof(NodeId_t));
  srcPort->readBytes((byte *) &dest, sizeof(NodeId_t));
  srcPort->readBytes((byte *) &payloadSize, sizeof(MessageSize_t));
  srcPort->readBytes((byte *) &sysCommand, sizeof(SysCommand_t));
  body = new byte[payloadSize];
  srcPort->readBytes(body, payloadSize);
}

void processMessage(Stream *srcPort, NodeId_t source, NodeId_t dest, MessageSize_t payloadSize, SysCommand_t sysCommand, byte *message) {
  char buff[100];
  if (sysCommand & GET_ID) {
    // Get ID is usually only used when the sender doesn't know the ID of the node, so just send it back the same srcPort
    sprintf(buff, "Node ID requested by %lu", source);
    Serial.println(buff);
    srcPort->write((char *) &NODE_ID, sizeof(NODE_ID));
    return;
  }
  if (dest != NODE_ID) {
    routeMessage(srcPort, source, dest, payloadSize, sysCommand, message);
    return;
  }
  if (sysCommand & UPDATE_NEIGHBORS) {

  }
  if (sysCommand & GET_NEIGHBORS) {
    sprintf(buff, "Node Neighbors requested by %lu", source);
    int maxRetries = 2 * (LISTEN_WAIT * 8 / PING_DELAY);
    for (int i = 0; i < 6; i++) {
      NEIGHBORS[i]->listen();
      if (neighborIds[i] == EMPTY && ackWait(NEIGHBORS[i], 20)) {
        writeMessage(NEIGHBORS[i], NODE_ID, EMPTY, 0, GET_ID, NULL);
        NEIGHBORS[i]->readBytes((byte *) neighborIds[i], sizeof(NodeId_t));
      }
    }
    routeMessage(srcPort, dest, source, sizeof(neighborIds), UPDATE_NEIGHBORS, (byte *) neighborIds);
  }
}

void routeMessage(Stream *srcPort, NodeId_t source, NodeId_t dest, MessageSize_t payloadSize, SysCommand_t sysCommand, byte *message) {
  char buff[100];
  sprintf(buff, "Routing message from %lu to %lu via %lu size %hu", source, dest, NODE_ID, payloadSize);
  Serial.println(buff);
  if (dest == PORT_H) {
    writeMessage(&Serial, source, PORT_H, payloadSize, sysCommand, message);
    return;
  }
}

void writeMessage(Stream *destPort, NodeId_t source, NodeId_t dest, MessageSize_t payloadSize, SysCommand_t sysCommand, byte *message) {
  destPort->write(START_CODE);
  destPort->write((char *) &source, sizeof(source));
  destPort->write((char *) &dest, sizeof(dest));
  destPort->write((char *) &payloadSize, sizeof(payloadSize));
  destPort->write((char *) &sysCommand, sizeof(sysCommand));
  destPort->write(message, payloadSize);
}

int strcicmp(char const *a, char const *b)
{
  for (;; a++, b++) {
    int d = tolower((unsigned char) * a) - tolower((unsigned char) * b);
    if (d != 0 || !*a)
      return d;
  }
}

NodeId_t getNodeId() {
  NodeId_t nodeId = 0;

  int datelen = strlen(__DATE__);
  int timelen = strlen(__TIME__);
  char date[datelen + 1] = {0};
  char time_[timelen + 1] = {0};
  strncpy(date, __DATE__, datelen);
  strncpy(time_, __TIME__, timelen);
  char *token;

  // add date to node ID
  int year, month, day;
  token = strtok(date, " :");
  day = atoi(strtok(NULL, " :"));
  year = atoi(strtok(NULL, " :"));

  nodeId += year;

  nodeId = nodeId << 4;
  if (strcicmp(token, "jan") == 0) {
    nodeId += 1;
  } else if (strcicmp(token, "feb") == 0) {
    nodeId += 2;
  } else if (strcicmp(token, "mar") == 0) {
    nodeId += 3;
  } else if (strcicmp(token, "apr") == 0) {
    nodeId += 4;
  } else if (strcicmp(token, "may") == 0) {
    nodeId += 5;
  } else if (strcicmp(token, "jun") == 0) {
    nodeId += 6;
  } else if (strcicmp(token, "jul") == 0) {
    nodeId += 7;
  } else if (strcicmp(token, "aug") == 0) {
    nodeId += 8;
  } else if (strcicmp(token, "sep") == 0) {
    nodeId += 9;
  } else if (strcicmp(token, "oct") == 0) {
    nodeId += 10;
  } else if (strcicmp(token, "nov") == 0) {
    nodeId += 11;
  } else if (strcicmp(token, "dec") == 0) {
    nodeId += 12;
  }

  nodeId = nodeId << 5;
  nodeId += day;

  // add time to node ID
  token = strtok(time_, " :");
  for (int i = 0; i < 3 && token != NULL; i++) {
    nodeId = nodeId << 6;
    nodeId += atoi(token);
    token = strtok(NULL, " :");
  }

  return nodeId;
}

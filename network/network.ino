#include <SoftwareSerial.h>

#define PING byte(0xAA)
#define ACK byte(0xAB)
#define START_CODE byte(0xAC)
#define READ_DELAY 50
#define PING_DELAY 1
#define LISTEN_WAIT 2

// Hardware serial has a special case address
#define PORT_H 0xffffffff
#define EMPTY 0x00000000
// System commands
#define GET_ID 0x01
#define GET_NEIGHBORS 0x02
#define GET_TOPOLOGY 0x04
#define UPDATE_TOPOLOGY 0x08


/*
 * Protocol
 * --------
 * 
 *  0                   1                   2                   3  
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Start Code  |                 Source Address                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |               |              Destination Address              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |               |         Payload Length        | System Command|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
 * 
 * Start Code:  8 bits
 *   Used to indicate start of message
 *   
 * Source Address:  32 bits
 *   The source address
 *   
 * Destination Address: 32 bits
 *   The destination address
 *   
 * Payload Length: 16 bits
 *   Size of the apyload after the header in bytes
 * 
 * System Command: 8 bits
 *   Specifies network commands to perform
 *   
 * System Commands
 * ---------------
 * Get ID: 0x01
 *   Retrieve the ID of the node. During newtork discovery, this can be used
 *   to determine the ID of one's neightbors
 *   
 * Get Neighbors: 0x02
 *   Retrieve the neighbors of the Source address
 *   
 * Update Network Topology: 0x04
 *   The payload contains updated network topology to use for routing
 *   
 * Get Network Topology: 0x08
 *   Return the node's network topology
 */

/*  
 * Each node has a SoftwareSerial connection to its neighbor and another to the
 * 1 /\ 2
 * 0 | | 3
 * 5 \/ 4
 * 
 * The 7th port (PORT_ACTOR) sends/receves messages through the network
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

Stream *NEIGHBORS[6] = {&PORT_0, &PORT_1, &PORT_2, &PORT_3, &PORT_4, &PORT_5};
Stream *ALLPORTS[7] = {&PORT_0, &PORT_1, &PORT_2, &PORT_3, &PORT_4, &PORT_5, &PORT_ACTOR};
uint32_t neighbors[6] = { EMPTY };

uint32_t NODE_ID;

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
  digitalWrite(STATUS_LED, HIGH);
  delay(1000);
  digitalWrite(STATUS_LED, LOW);
}

void loop() {
  uint32_t source, dest;
  uint16_t payloadSize;
  uint8_t sysCommand;
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

void ackWait(Stream *port) {
  bool connected = false;
  char ackBuff[1];
  while (!connected) {
    Serial.println("pinging");
    port->write(PING);
    byte pong = port->read();
    Serial.println(pong, HEX);
    if (pong == ACK) {
      break;
    }
    delay(PING_DELAY);
  }
}

bool hasIncoming(Stream *port) {
  delay(LISTEN_WAIT);
  byte resp = port->read();
  if (resp == PING) {
    Serial.println("incoming data");
    port->write(ACK);
    return true;
  }
  return false;
}

void readMessage(Stream *port, uint32_t &source, uint32_t &dest, uint16_t &payloadSize, uint8_t &sysCommand, byte *&body) {
  byte startByte;
  do {
    port->readBytes(&startByte, 1);
  } while (startByte != START_CODE);
  port->readBytes((byte *) &source, 4);
  port->readBytes((byte *) &dest, 4);
  port->readBytes((byte *) &payloadSize, 2);
  port->readBytes((byte *) &sysCommand, 1);
  Serial.println(startByte, HEX);
  Serial.println(source, HEX);
  Serial.println(dest, HEX);
  Serial.println(payloadSize, HEX);
  Serial.println(sysCommand, HEX);
  body = new byte[payloadSize];
  port->readBytes(body, payloadSize);
}

void processMessage(Stream *port, uint32_t &source, uint32_t &dest, uint16_t &payloadSize, uint8_t &sysCommand, byte *&message) {
  if (sysCommand | GET_ID) {
    // Get ID is usually only used when the sender doesn't know the ID of the node, so just send it back the same port
    port->write((char *) &NODE_ID, sizeof(NODE_ID));
  }
  if (sysCommand | GET_NEIGHBORS) {
    
  }
}

int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

uint32_t getNodeId() {
  uint32_t nodeId = 0;
 
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

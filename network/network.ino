#include <SoftwareSerial.h>

#define PING byte(0x00)
#define ACK byte(0xAB)
#define READ_DELAY 50
#define PING_DELAY 1
#define LISTEN_WAIT 2

// Hardware serial has a special case address
#define PORT_H 0xffffffffffffffff

/*
 * Protocol
 * --------
 * 
 * 0                   1                   2                   3  
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Start Code  |                                               |
 * +-+-+-+-+-+-+-+-+                                               +
 * |                         Source Address                        |
 * +               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |               |                                               |
 * +-+-+-+-+-+-+-+-+                                               +
 * |                       Destination Address                     |
 * +               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |               |         Payload Length        | System Command|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 * Start Code:  8 bits
 *   Used to indicate start of message
 *   
 * Source Address:  64 bits
 *   The source address
 *   
 * Destination Address: 64 bits
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
//Status LED
#define RED A2
#define GREEN A3
#define BLUE A4
SoftwareSerial PORT_0(2, 3);
SoftwareSerial PORT_1(4, 5);
SoftwareSerial PORT_2(6, 7);
SoftwareSerial PORT_3(8, 9);
SoftwareSerial PORT_4(10, 11);
SoftwareSerial PORT_5(12, 13);
SoftwareSerial PORT_ACTOR(14, 15);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//Status LED
#define RED A0
#define GREEN A1
#define BLUE A2
SoftwareSerial PORT_0(10, 11);
SoftwareSerial PORT_1(12, 13);
SoftwareSerial PORT_2(50, 51);
SoftwareSerial PORT_3(52, 53);
SoftwareSerial PORT_4(62, 63);
SoftwareSerial PORT_5(65, 66);
SoftwareSerial PORT_ACTOR(67, 68);
#endif

SoftwareSerial *ALLPORTS[7] = {&PORT_0, &PORT_1, &PORT_2, &PORT_3, &PORT_4, &PORT_5, &PORT_ACTOR};

uint64_t NODE_ID;

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
  setStatusLED(0x00, 0xff, 0x00);
  delay(1000);
  setStatusLED(0x00, 0x00, 0x00);
}

void loop() {
  // Loop through ports and process messages
  if (Serial.available() > 0) {
    int len = Serial.read() - 48;
    char msg[len + 1] = {0};
    Serial.readBytes(msg, len);
    Serial.println(msg);
    ackWait(&PORT_0);
    PORT_0.write(msg, len);
  }

  for (int i = 0; i < 7; i++) {
    char buff[50];
    SoftwareSerial *port = ALLPORTS[i];
    if (hasIncoming(port)) {
      Serial.println("recieving message");
      delay(READ_DELAY);
      while (port->available()) {
        Serial.write(port->read());
      }
    }
  }
}

void ackWait(SoftwareSerial *port) {
  port->listen();
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

bool hasIncoming(SoftwareSerial *port) {
  port->listen();
  delay(LISTEN_WAIT);
  byte resp = port->read();
  if (resp == PING) {
    Serial.println("incoming data");
    port->write(ACK);
    // consume extra pings
    while(port->peek() == PING) {
      Serial.println("strip extra pings");
      port->read();
    }
    return true;
  }
  return false;
}

int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

uint64_t getNodeId() {
  uint64_t nodeId = 0;
 
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

void setStatusLED(byte r, byte g, byte b) {
  analogWrite(RED, r);
  analogWrite(GREEN, g);
  analogWrite(BLUE, b);
}

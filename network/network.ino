#include <SoftwareSerial.h>

#define PING byte(0x00)
#define ACK byte(0xAB)
#define READ_DELAY 50
#define PING_DELAY 1
#define LISTEN_WAIT 2

// Used as a hopefully unique ID for each node in the network
const char NODE_ID[] = "*" __DATE__ "*" __TIME__ "*";

/*  
 * Each node has a SoftwareSerial connection to its neighbor and another to the
 * 1 /\ 2
 * 0 | | 3
 * 5 \/ 4
 * 
 * The 7th port (PORT_ACTOR) sends/receves messages through the network
 */
#if defined(__AVR_ATmega328P__)
SoftwareSerial PORT_0(2, 3);
SoftwareSerial PORT_1(4, 5);
SoftwareSerial PORT_2(6, 7);
SoftwareSerial PORT_3(8, 9);
SoftwareSerial PORT_4(10, 11);
SoftwareSerial PORT_5(12, 13);
SoftwareSerial PORT_ACTOR(14, 15);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
SoftwareSerial PORT_0(10, 11);
SoftwareSerial PORT_1(12, 13);
SoftwareSerial PORT_2(50, 51);
SoftwareSerial PORT_3(52, 53);
SoftwareSerial PORT_4(62, 63);
SoftwareSerial PORT_5(65, 66);
SoftwareSerial PORT_ACTOR(67, 68);
#endif

SoftwareSerial *ALLPORTS[7] = {&PORT_0, &PORT_1, &PORT_2, &PORT_3, &PORT_4, &PORT_5, &PORT_ACTOR};

void setup() {
  Serial.begin(9600);
  PORT_0.begin(9600);
  PORT_1.begin(9600);
  PORT_2.begin(9600);
  PORT_3.begin(9600);
  PORT_4.begin(9600);
  PORT_5.begin(9600);
  PORT_ACTOR.begin(9600);
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

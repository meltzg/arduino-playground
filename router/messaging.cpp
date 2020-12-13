#include <Arduino.h>
#include "messaging.h"


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

void writeMessage(Stream *destPort, NodeId_t source, NodeId_t dest, MessageSize_t payloadSize, SysCommand_t sysCommand, byte *message) {
  destPort->write(START_CODE);
  destPort->write((char *) &source, sizeof(source));
  destPort->write((char *) &dest, sizeof(dest));
  destPort->write((char *) &payloadSize, sizeof(payloadSize));
  destPort->write((char *) &sysCommand, sizeof(sysCommand));
  destPort->write(message, payloadSize);
}

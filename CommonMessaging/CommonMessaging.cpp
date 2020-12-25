#include <Arduino.h>
#include "CommonMessaging.h"


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

Message readMessage(Stream *srcPort) {
  byte startByte;
  do {
    srcPort->readBytes(&startByte, sizeof(StartCode_t));
  } while (startByte != START_CODE);
  Message message;
  srcPort->readBytes((byte *) & (message.source), sizeof(NodeId_t));
  srcPort->readBytes((byte *) & (message.dest), sizeof(NodeId_t));
  srcPort->readBytes((byte *) & (message.payloadSize), sizeof(MessageSize_t));
  srcPort->readBytes((byte *) & (message.sysCommand), sizeof(SysCommand_t));
  message.body = new byte[message.payloadSize];
  srcPort->readBytes(message.body, message.payloadSize);
  return message;
}

void writeMessage(Stream *destPort, const Message &message) {
  destPort->write(START_CODE);
  destPort->write((char *) & (message.source), sizeof(message.source));
  destPort->write((char *) & (message.dest), sizeof(message.dest));
  destPort->write((char *) & (message.payloadSize), sizeof(message.payloadSize));
  destPort->write((char *) & (message.sysCommand), sizeof(message.sysCommand));
  destPort->write(message.body, message.payloadSize);
}
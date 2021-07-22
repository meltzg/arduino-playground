#include <Arduino.h>
#include "CommonMessaging.h"

void Message::free() {
    delete[] body;
    body = NULL;
}

bool ackWait(Stream *port, int maxRetries = -1)
{
    bool connected = false;
    char ackBuff[1];
    for (int i = 0; !connected && (i < maxRetries || maxRetries < 0); i++)
    {
        port->write(PING_BYTE);
        byte pong = port->read();
        if (pong == ACK_BYTE)
        {
            return true;
        }
    }
    return false;
}

bool hasIncoming(Stream *port)
{
    long start = millis();
    while (millis() - start < LISTEN_WAIT)
    {
        if (port->available() && port->read() == PING_BYTE)
        {
            port->write(ACK_BYTE);
            return true;
        }
    }
    return false;
}

Message readMessage(Stream *srcPort)
{
    Serial.print(millis());
    Serial.println(F(": rxmsg"));
    byte startByte;
    do
    {
        srcPort->readBytes(&startByte, sizeof(StartCode_t));
    } while (startByte != START_CODE);
    Message message;
    srcPort->readBytes((byte *)&message, sizeof(Message));
    byte *body = new byte[message.getPayloadSize()];
    srcPort->readBytes(body, message.getPayloadSize());
    message.setBody(body);

    return message;
}

void writeMessage(Stream *destPort, const Message &message)
{
    Serial.print(millis());
    Serial.println(F(": txmsg"));
    destPort->write(START_CODE);
    destPort->write((char *)&message, sizeof(Message));
    destPort->write(message.getBody(), message.getPayloadSize());
}

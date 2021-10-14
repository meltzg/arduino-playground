#include <Arduino.h>
#include "CommonMessaging.h"

void Message::free()
{
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
            // send the start code and consume any remaining bytes from the destination
            port->write(START_CODE);
            while (port->read() >= 0)
                ;
            return true;
        }
    }
    return false;
}

bool hasIncoming(Stream *port)
{
    long start = millis();
    StartCode_t startByte;
    while (millis() - start < LISTEN_WAIT)
    {
        if (port->available() && port->read() == PING_BYTE)
        {
            do
            {
                port->write(ACK_BYTE);
                port->readBytes(&startByte, sizeof(StartCode_t));
            } while (startByte != START_CODE);

            return true;
        }
    }
    return false;
}

Message readMessage(Stream *srcPort)
{
    Message message;

    if (hasIncoming(srcPort))
    {
        Serial.print(millis());
        Serial.println(F(": rxmsg"));
        srcPort->readBytes((byte *)&message, sizeof(Message));
        byte *body = new byte[message.getPayloadSize()];
        srcPort->readBytes(body, message.getPayloadSize());
        message.setBody(body);

        Serial.print(F("src "));
        Serial.println(message.getSource(), HEX);
        Serial.print(F("dst "));
        Serial.println(message.getDest(), HEX);
        Serial.print(F("size "));
        Serial.println(message.getPayloadSize());
        Serial.print(F("op "));
        Serial.println(message.getSysOption(), HEX);
        Serial.print(F("cmd "));
        Serial.println(message.getSysCommand(), HEX);
    }

    return message;
}

bool writeMessage(Stream *destPort, const Message &message, int maxWaitRetries)
{
    if (ackWait(destPort, maxWaitRetries))
    {
        destPort->write((char *)&message, sizeof(Message));
        destPort->write(message.getBody(), message.getPayloadSize());
        Serial.print(millis());
        Serial.println(F(": txmsg"));
        return true;
    }
    return false;
}

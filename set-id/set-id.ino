#include <EEPROM.h>
#include "CommonMessaging.h"


void setup()
{
    Serial.begin(9600);
    NodeId_t id = MY_ID;
    EEPROM.put(0, MY_ID);
}

void loop()
{
    Serial.print(MY_ID, HEX);
    Serial.println(F(" Complete"));
}

#include <EEPROM.h>
#include "CommonMessaging.h"

#ifndef EEPROM_OFFSET
#define EEPROM_OFFSET 0
#endif

#define PROGRESS_STEP 5

int previousPercent = 0;

void setup()
{
    Serial.begin(9600);

    int percentComplete = 0;
    int total = EEPROM.length();
    Serial.print("EEPROM size: ");
    Serial.println(total);
    for (int i = EEPROM_OFFSET; i < total; i++)
    {
        EEPROM.update(i, 0);
        int currentPercent = (((float)i) / total) * 100;
        if (currentPercent != previousPercent && currentPercent % PROGRESS_STEP == 0)
        {
            previousPercent = currentPercent;
            Serial.print(currentPercent);
            Serial.println("\% Cleared");
        }
    }
}

void loop()
{
    Serial.println("Complete");
}

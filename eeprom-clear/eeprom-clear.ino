#include <EEPROM.h>

#define PROGRESS_STEP 5

int previousPercent = 0;

void setup()
{
    Serial.begin(9600);

    int percentComplete = 0;
    int total = EEPROM.length();
    Serial.print("EEPROM size: ");
    Serial.println(total);
    for (int i = 0; i < total; i++)
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

    Serial.println("Complete");
}

void loop()
{
}

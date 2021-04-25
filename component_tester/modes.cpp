#include "modes.h"

void ComponentTestMode::init()
{
    disp.setRenderChars(false);
}

void ComponentTestMode::process(unsigned long currentMillis)
{
    uint16_t btnState = btns.getState();

    static int colorOffset = 0;

    if (currentMillis - previousMillis > 500)
    {
        __int24 ledColors[leds.getNumLeds()] = {BLACK};
        for (int i = 0; i < leds.getNumLeds(); i++)
        {
            if (((1 << i) & btnState) > 0)
            {
                continue;
            }
            ledColors[i] = RAINBOW[(i + colorOffset) % 6];
        }
        colorOffset++;
        leds.setState(ledColors);
        disp.registerWrite(~(1 << (colorOffset % 16)));

        previousMillis = currentMillis;
    }
}

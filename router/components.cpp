#include "components.h"


void StatusLed::render(unsigned long currentMillis)
{
  if ((currentMillis - previousMillis) >= LED_UPDATE_DELAY) {
    pixels.clear();
    pixels.fill(grb);
    pixels.show();
    previousMillis = currentMillis;
  }
}
void StatusLed::set(__uint24 grb, byte brightness, int duration)
{
  if (this->grb != grb || this->brightness != brightness) {
    this->grb = grb;
    this->brightness = brightness;
  }
}

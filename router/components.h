#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_
#include <Adafruit_NeoPixel.h>

#define LED_UPDATE_DELAY 100


class StatusLed {
  private:
    __int24 grb;
    byte brightness;
    const Adafruit_NeoPixel pixels;
    const int dataPin;

    unsigned long previousMillis = 0;

  public:
    StatusLed(int dataPin) : dataPin(dataPin), grb(0x000000), brightness(123) {
      pinMode(dataPin, OUTPUT);
      pixels = Adafruit_NeoPixel(1, dataPin, NEO_GRB + NEO_KHZ800);
      pixels.begin();

    }

    void render(unsigned long currentMillis);
    void set(__uint24 grb, byte brightness, int duration);
};

#endif // _COMPONENTS_H_

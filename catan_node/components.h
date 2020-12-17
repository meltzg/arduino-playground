#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include <Adafruit_NeoPixel.h>


#define RED 0x00FF00
#define ORANGE 0xA5FF00
#define YELLOW 0xFFFF00
#define GREEN 0x800000
#define BLUE 0x0000FF
#define PURPLE 0x008080

class Component {
  protected:
    unsigned long previousMillis = 0;

  public:
    virtual void render(unsigned long currentMillis);
};

class SegmentDisplay : public Component
{
  private:
    const static int digitSwitchDelay = 5;

    const int latchPin;
    const int clockPin;
    const int dataPin;
    const int leftCommonPin;
    const int rightCommonPin;

    char chars[2] = {0};
    bool showLeft = true;

    void registerWrite(uint16_t toWrite);

  public:
    SegmentDisplay(int latchPin, int clockPin, int dataPin, int leftCommonPin, int rightCommonPin) : latchPin(latchPin), clockPin(clockPin), dataPin(dataPin), leftCommonPin(leftCommonPin), rightCommonPin(rightCommonPin) {}

    void setChars(char *chars);
    void render(unsigned long currentMillis);
};

class LEDStatusDisplay : public Component
{
  private:
    const static int updateDelay = 100;

    const int dataPin;
    byte shiftCounter = 0;
    Adafruit_NeoPixel pixels;

  public:
    LEDStatusDisplay(int dataPin);
  
    void render(unsigned long currentMillis);
};

#endif // _COMPONENTS_H_

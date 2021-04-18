#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include <Adafruit_NeoPixel.h>

#define DIGIT_SWITCH_DELAY 5
#define SEGMENT_SCROLL_DELAY 250
#define BTN_POLL_DELAY 50
#define PULSE_WIDTH_USEC 5
#define LED_UPDATE_DELAY 100

#define RED 0x00FF00
#define ORANGE 0xA5FF00
#define YELLOW 0xFFFF00
#define GREEN 0x800000
#define BLUE 0x0000FF
#define PURPLE 0x008080
#define BLACK 0x00
#define WHITE 0xFFFFFF

const __int24 RAINBOW[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE};

class Component
{
protected:
  unsigned long previousMillis = 0;

public:
  virtual void render(unsigned long currentMillis);
};

class SegmentDisplay : public Component
{
private:
  const int latchPin;
  const int clockPin;
  const int dataPin;
  const int leftCommonPin;
  const int rightCommonPin;
  const int scrollDelay;
  bool renderChars;

  char *chars = NULL;
  bool showLeft = true;
  int displayOffset = 0;
  unsigned long scrollPreviousMillis = 0;

public:
  SegmentDisplay(int latchPin, int clockPin, int dataPin, int leftCommonPin, int rightCommonPin, int scrollDelay, bool renderChars=true);

  void registerWrite(uint16_t toWrite);
  void setChars(char *chars);
  void setRenderChars(bool renderChars) { this->renderChars = renderChars; }
  void render(unsigned long currentMillis);
};

class LEDStatusDisplay : public Component
{
private:
  const int dataPin;
  const int numLeds;
  Adafruit_NeoPixel pixels;

  __int24 *grbs = NULL;
  byte brightness = 50;

public:
  LEDStatusDisplay(int dataPin, int numLeds);

  void render(unsigned long currentMillis);
  void setState(__int24 *grbs, byte brightness);
};

class ButtonArray16 : Component
{
private:
  const int loadPin;
  const int clockEnablePin;
  const int dataPin;
  const int clockPin;

  uint16_t state = 0;
  uint16_t onDuration[16] = {0};

public:
  ButtonArray16(int loadPin, int clockEnablePin, int dataPin, int clockPin);

  void render(unsigned long currentMillis);
  uint16_t getState() { return state; }
  uint16_t getOnDuration(int pos) { return onDuration[pos]; }
};

#endif // _COMPONENTS_H_

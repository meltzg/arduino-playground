#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_
class SegmentDisplay
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

    unsigned long previousMillis = 0;

    void registerWrite(uint16_t toWrite);

  public:
    SegmentDisplay(int latchPin, int clockPin, int dataPin, int leftCommonPin, int rightCommonPin) : latchPin(latchPin), clockPin(clockPin), dataPin(dataPin), leftCommonPin(leftCommonPin), rightCommonPin(rightCommonPin) {}
    
    void setChars(char *chars);
    void render(unsigned long currentMillis);
};

#endif // _COMPONENTS_H_

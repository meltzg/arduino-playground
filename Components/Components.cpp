#include <Arduino.h>
#include <Components.h>

#define __SEG_16__
#define __COMMON_ANODE__

#ifdef __COMMON_ANODE__
#define SEG_ON HIGH
#define SEG_OFF LOW
#else
#define SEG_ON LOW
#define SEG_OFF HIGH
#endif

const uint16_t getCharacter(char character)
{
  uint16_t charCode = 0;
  switch (character)
  {
#ifdef __SEG_16__
  case '!':
    charCode = 0x000C; /* ! */
    break;
  case '"':
    charCode = 0x0204; /* " */
    break;
  case '#':
    charCode = 0xAA3C; /* # */
    break;
  case '$':
    charCode = 0xAABB; /* $ */
    break;
  case '%':
    charCode = 0xEE99; /* % */
    break;
  case '&':
    charCode = 0x9371; /* & */
    break;
  case '\'':
    charCode = 0x0200; /* ' */
    break;
  case '(':
    charCode = 0x1400; /* ( */
    break;
  case ')':
    charCode = 0x4100; /* ) */
    break;
  case '*':
    charCode = 0xFF00; /* * */
    break;
  case '+':
    charCode = 0xAA00; /* + */
    break;
  case ',':
    charCode = 0x4000; /* ; */
    break;
  case '-':
    charCode = 0x8800; /* - */
    break;
  case '.':
    charCode = 0x1000; /* . */
    break;
  case '/':
    charCode = 0x4400; /* / */
    break;
  case '0':
    charCode = 0x44FF; /* 0 */
    break;
  case '1':
    charCode = 0x040C; /* 1 */
    break;
  case '2':
    charCode = 0x8877; /* 2 */
    break;
  case '3':
    charCode = 0x083F; /* 3 */
    break;
  case '4':
    charCode = 0x888C; /* 4 */
    break;
  case '5':
    charCode = 0x90B3; /* 5 */
    break;
  case '6':
    charCode = 0x88FB; /* 6 */
    break;
  case '7':
    charCode = 0x000F; /* 7 */
    break;
  case '8':
    charCode = 0x88FF; /* 8 */
    break;
  case '9':
    charCode = 0x88BF; /* 9 */
    break;
  case ':':
    charCode = 0x2200; /* : */
    break;
  case ';':
    charCode = 0x4200; /* ; */
    break;
  case '<':
    charCode = 0x9400; /* < */
    break;
  case '=':
    charCode = 0x8830; /* = */
    break;
  case '>':
    charCode = 0x4900; /* > */
    break;
  case '?':
    charCode = 0x2807; /* ? */
    break;
  case '@':
    charCode = 0x0AF7; /* @ */
    break;
  case 'A':
    charCode = 0x88CF; /* A */
    break;
  case 'B':
    charCode = 0x2A3F; /* B */
    break;
  case 'C':
    charCode = 0x00F3; /* C */
    break;
  case 'D':
    charCode = 0x223F; /* D */
    break;
  case 'E':
    charCode = 0x80F3; /* E */
    break;
  case 'F':
    charCode = 0x80C3; /* F */
    break;
  case 'G':
    charCode = 0x08FB; /* G */
    break;
  case 'H':
    charCode = 0x88CC; /* H */
    break;
  case 'I':
    charCode = 0x2233; /* I */
    break;
  case 'J':
    charCode = 0x007C; /* J */
    break;
  case 'K':
    charCode = 0x94C0; /* K */
    break;
  case 'L':
    charCode = 0x00F0; /* L */
    break;
  case 'M':
    charCode = 0x05CC; /* M */
    break;
  case 'N':
    charCode = 0x11CC; /* N */
    break;
  case 'O':
    charCode = 0x00FF; /* O */
    break;
  case 'P':
    charCode = 0x88C7; /* P */
    break;
  case 'Q':
    charCode = 0x10FF; /* Q */
    break;
  case 'R':
    charCode = 0x98C7; /* R */
    break;
  case 'S':
    charCode = 0x88BB; /* S */
    break;
  case 'T':
    charCode = 0x2203; /* T */
    break;
  case 'U':
    charCode = 0x00FC; /* U */
    break;
  case 'V':
    charCode = 0x44C0; /* V */
    break;
  case 'W':
    charCode = 0x50CC; /* W */
    break;
  case 'X':
    charCode = 0x5500; /* X */
    break;
  case 'Y':
    charCode = 0x88BC; /* Y */
    break;
  case 'Z':
    charCode = 0x4433; /* Z */
    break;
  case '[':
    charCode = 0x2212; /* [ */
    break;
  case ']':
    charCode = 0x1100; /* \ */
    break;
  case '\\':
    charCode = 0x2221; /* ] */
    break;
  case '^':
    charCode = 0x5000; /* ^ */
    break;
  case '_':
    charCode = 0x0030; /* _ */
    break;
  case '`':
    charCode = 0x0100; /* ` */
    break;
  case 'a':
    charCode = 0xA070; /* a */
    break;
  case 'b':
    charCode = 0xA0E0; /* b */
    break;
  case 'c':
    charCode = 0x8060; /* c */
    break;
  case 'd':
    charCode = 0x281C; /* d */
    break;
  case 'e':
    charCode = 0xC060; /* e */
    break;
  case 'f':
    charCode = 0xAA02; /* f */
    break;
  case 'g':
    charCode = 0xA2A1; /* g */
    break;
  case 'h':
    charCode = 0xA0C0; /* h */
    break;
  case 'i':
    charCode = 0x2000; /* i */
    break;
  case 'j':
    charCode = 0x2260; /* j */
    break;
  case 'k':
    charCode = 0x3600; /* k */
    break;
  case 'l':
    charCode = 0x00C0; /* l */
    break;
  case 'm':
    charCode = 0xA848; /* m */
    break;
  case 'n':
    charCode = 0xA040; /* n */
    break;
  case 'o':
    charCode = 0xA060; /* o */
    break;
  case 'p':
    charCode = 0x82C1; /* p */
    break;
  case 'q':
    charCode = 0xA281; /* q */
    break;
  case 'r':
    charCode = 0x8040; /* r */
    break;
  case 's':
    charCode = 0xA0A1; /* s */
    break;
  case 't':
    charCode = 0x80E0; /* t */
    break;
  case 'u':
    charCode = 0x2060; /* u */
    break;
  case 'v':
    charCode = 0x4040; /* v */
    break;
  case 'w':
    charCode = 0x5048; /* w */
    break;
  case 'x':
    charCode = 0x5500; /* x */
    break;
  case 'y':
    charCode = 0x0A1C; /* y */
    break;
  case 'z':
    charCode = 0xC020; /* z */
    break;
  case '{':
    charCode = 0xA212; /* { */
    break;
  case '|':
    charCode = 0x2200; /* | */
    break;
  case '}':
    charCode = 0x2A21; /* } */
    break;
  case '~':
    charCode = 0xCC00; /* ~ */
    break;
  default:
    charCode = 0x0000; /* space */
    break;
#else
  case '!':
    charCode = 0x4006; /* ! */
    break;
  case '"':
    charCode = 0x0202; /* " */
    break;
  case '#':
    charCode = 0x12CE; /* # */
    break;
  case '$':
    charCode = 0x12ED; /* $ */
    break;
  case '%':
    charCode = 0x3FE4; /* % */
    break;
  case '&':
    charCode = 0x2359; /* & */
    break;
  case '\'':
    charCode = 0x0200; /* ' */
    break;
  case '(':
    charCode = 0x2400; /* ( */
    break;
  case ')':
    charCode = 0x0900; /* ) */
    break;
  case '*':
    charCode = 0x3FC0; /* * */
    break;
  case '+':
    charCode = 0x12C0; /* + */
    break;
  case ',':
    charCode = 0x0800; /* , */
    break;
  case '-':
    charCode = 0x00C0; /* - */
    break;
  case '.':
    charCode = 0x4000; /* . */
    break;
  case '/':
    charCode = 0x0C00; /* / */
    break;
  case '0':
    charCode = 0x0C3F; /* 0 */
    break;
  case '1':
    charCode = 0x0406; /* 1 */
    break;
  case '2':
    charCode = 0x00DB; /* 2 */
    break;
  case '3':
    charCode = 0x008F; /* 3 */
    break;
  case '4':
    charCode = 0x00E6; /* 4 */
    break;
  case '5':
    charCode = 0x2069; /* 5 */
    break;
  case '6':
    charCode = 0x00FD; /* 6 */
    break;
  case '7':
    charCode = 0x0007; /* 7 */
    break;
  case '8':
    charCode = 0x00FF; /* 8 */
    break;
  case '9':
    charCode = 0x00EF; /* 9 */
    break;
  case ':':
    charCode = 0x1200; /* : */
    break;
  case ';':
    charCode = 0x0A00; /* ; */
    break;
  case '<':
    charCode = 0x2440; /* < */
    break;
  case '=':
    charCode = 0x00C8; /* = */
    break;
  case '>':
    charCode = 0x0980; /* > */
    break;
  case '?':
    charCode = 0x5083; /* ? */
    break;
  case '@':
    charCode = 0x02BB; /* @ */
    break;
  case 'A':
    charCode = 0x00F7; /* A */
    break;
  case 'B':
    charCode = 0x128F; /* B */
    break;
  case 'C':
    charCode = 0x0039; /* C */
    break;
  case 'D':
    charCode = 0x120F; /* D */
    break;
  case 'E':
    charCode = 0x0079; /* E */
    break;
  case 'F':
    charCode = 0x0071; /* F */
    break;
  case 'G':
    charCode = 0x00BD; /* G */
    break;
  case 'H':
    charCode = 0x00F6; /* H */
    break;
  case 'I':
    charCode = 0x1209; /* I */
    break;
  case 'J':
    charCode = 0x001E; /* J */
    break;
  case 'K':
    charCode = 0x2470; /* K */
    break;
  case 'L':
    charCode = 0x0038; /* L */
    break;
  case 'M':
    charCode = 0x0536; /* M */
    break;
  case 'N':
    charCode = 0x2136; /* N */
    break;
  case 'O':
    charCode = 0x003F; /* O */
    break;
  case 'P':
    charCode = 0x00F3; /* P */
    break;
  case 'Q':
    charCode = 0x203F; /* Q */
    break;
  case 'R':
    charCode = 0x20F3; /* R */
    break;
  case 'S':
    charCode = 0x00ED; /* S */
    break;
  case 'T':
    charCode = 0x1201; /* T */
    break;
  case 'U':
    charCode = 0x003E; /* U */
    break;
  case 'V':
    charCode = 0x0C30; /* V */
    break;
  case 'W':
    charCode = 0x2836; /* W */
    break;
  case 'X':
    charCode = 0x2D00; /* X */
    break;
  case 'Y':
    charCode = 0x00EE; /* Y */
    break;
  case 'Z':
    charCode = 0x0C09; /* Z */
    break;
  case '[':
    charCode = 0x0039; /* [ */
    break;
  case '\\':
    charCode = 0x2100; /* \ */
    break;
  case ']':
    charCode = 0x000F; /* ] */
    break;
  case '^':
    charCode = 0x2800; /* ^ */
    break;
  case '_':
    charCode = 0x0008; /* _ */
    break;
  case '`':
    charCode = 0x0100; /* ` */
    break;
  case 'a':
    charCode = 0x1058; /* a */
    break;
  case 'b':
    charCode = 0x2078; /* b */
    break;
  case 'c':
    charCode = 0x00D8; /* c */
    break;
  case 'd':
    charCode = 0x088E; /* d */
    break;
  case 'e':
    charCode = 0x0858; /* e */
    break;
  case 'f':
    charCode = 0x14C0; /* f */
    break;
  case 'g':
    charCode = 0x048E; /* g */
    break;
  case 'h':
    charCode = 0x1070; /* h */
    break;
  case 'i':
    charCode = 0x1000; /* i */
    break;
  case 'j':
    charCode = 0x0A10; /* j */
    break;
  case 'k':
    charCode = 0x3600; /* k */
    break;
  case 'l':
    charCode = 0x0030; /* l */
    break;
  case 'm':
    charCode = 0x10D4; /* m */
    break;
  case 'n':
    charCode = 0x1050; /* n */
    break;
  case 'o':
    charCode = 0x00DC; /* o */
    break;
  case 'p':
    charCode = 0x0170; /* p */
    break;
  case 'q':
    charCode = 0x0486; /* q */
    break;
  case 'r':
    charCode = 0x0050; /* r */
    break;
  case 's':
    charCode = 0x2088; /* s */
    break;
  case 't':
    charCode = 0x0078; /* t */
    break;
  case 'u':
    charCode = 0x001C; /* u */
    break;
  case 'v':
    charCode = 0x0810; /* v */
    break;
  case 'w':
    charCode = 0x2814; /* w */
    break;
  case 'x':
    charCode = 0x2D00; /* x */
    break;
  case 'y':
    charCode = 0x028E; /* y */
    break;
  case 'z':
    charCode = 0x0848; /* z */
    break;
  case '{':
    charCode = 0x0949; /* { */
    break;
  case '|':
    charCode = 0x1200; /* | */
    break;
  case '}':
    charCode = 0x2489; /* } */
    break;
  case '~':
    charCode = 0x0CC0; /* ~ */
    break;
  default:
    charCode = 0x0000; /* (space) */
    break;
#endif
  }

#ifdef __COMMON_ANODE__
  charCode = ~charCode;
#endif
  return charCode;
}

SegmentDisplay::SegmentDisplay(int latchPin, int clockPin, int dataPin, int leftCommonPin, int rightCommonPin, int scrollDelay, bool renderChars) : latchPin(latchPin), clockPin(clockPin), dataPin(dataPin), leftCommonPin(leftCommonPin), rightCommonPin(rightCommonPin), scrollDelay(scrollDelay), renderChars(renderChars)
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(leftCommonPin, OUTPUT);
  pinMode(rightCommonPin, OUTPUT);

  if (!renderChars)
  {
    digitalWrite(leftCommonPin, SEG_ON);
    digitalWrite(rightCommonPin, SEG_ON);
  }
}

void SegmentDisplay::registerWrite(uint16_t toWrite)
{
  // the bits you want to send
  byte bitsHigh = 0, bitsLow = 0;
  // turn off the output so the pins don't light up
  // while you're shifting bits:
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, highByte(toWrite));
  shiftOut(dataPin, clockPin, MSBFIRST, lowByte(toWrite));
  // turn on the output so the LEDs can light up:
  digitalWrite(latchPin, HIGH);
}

void SegmentDisplay::setChars(char *chars)
{
  this->displayOffset = 0;
  this->chars = chars;
}

void SegmentDisplay::setRenderChars(bool renderChars)
{
  this->renderChars = renderChars;
  digitalWrite(leftCommonPin, SEG_ON);
  digitalWrite(rightCommonPin, SEG_ON);
}

void SegmentDisplay::render(unsigned long currentMillis)
{
  if (currentMillis - previousMillis >= DIGIT_SWITCH_DELAY)
  {
    showLeft = !showLeft;
    previousMillis = currentMillis;
    char left = ' ', right = ' ';
    if (chars != NULL && strlen(chars) > 0)
    {
      if (strlen(chars) > 2 && currentMillis - scrollPreviousMillis >= scrollDelay)
      {
        scrollPreviousMillis = currentMillis;
        displayOffset = ++displayOffset % strlen(chars);
      }

      left = chars[displayOffset];

      if (strlen(chars) > 1)
      {
        right = chars[(displayOffset + 1) % strlen(chars)];
      }
    }

    // Assume common anode. to turn just left on, right must be high, left must be low
    if (renderChars)
    {
      if (showLeft)
      {
        digitalWrite(rightCommonPin, SEG_OFF);
        registerWrite(getCharacter(left));
        digitalWrite(leftCommonPin, SEG_ON);
      }
      else
      {
        digitalWrite(leftCommonPin, SEG_OFF);
        registerWrite(getCharacter(right));
        digitalWrite(rightCommonPin, SEG_ON);
      }
    }
  }
}

LEDStatusDisplay::LEDStatusDisplay(int dataPin, int numLeds) : dataPin(dataPin), numLeds(numLeds)
{
  pixels = Adafruit_NeoPixel(numLeds, dataPin, NEO_GRB + NEO_KHZ800);

  pixels.begin();
}

void LEDStatusDisplay::setState(const __int24 *grbs)
{
    pixels.clear();
    pixels.setBrightness(BRIGHTNESS);
    for (int i = 0; i < numLeds; i++)
    {
        pixels.setPixelColor(i, grbs[i]);
    }
    pixels.show();
}

ButtonArray16::ButtonArray16(int loadPin, int clockEnablePin, int dataPin, int clockPin) : loadPin(loadPin), clockEnablePin(clockEnablePin), dataPin(dataPin), clockPin(clockPin)
{
  pinMode(loadPin, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);

  digitalWrite(clockPin, LOW);
  digitalWrite(loadPin, HIGH);
}

void ButtonArray16::render(unsigned long currentMillis)
{
  if (currentMillis - previousMillis >= BTN_POLL_DELAY)
  {
    long bitVal;
    this->state = 0;

    // Trigger a parallel Load to latch the state of the data lines,
    digitalWrite(clockEnablePin, HIGH);
    digitalWrite(loadPin, LOW);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(loadPin, HIGH);
    digitalWrite(clockEnablePin, LOW);

    for (int i = 0; i < 16; i++)
    {
      bitVal = digitalRead(dataPin);

      /* Set the corresponding bit in bytesVal.
      */
      this->state |= (bitVal << ((16 - 1) - i));

      /* Pulse the Clock (rising edge shifts the next bit).
      */
      digitalWrite(clockPin, HIGH);
      delayMicroseconds(PULSE_WIDTH_USEC);
      digitalWrite(clockPin, LOW);
    }

    for (int i = 0; i < 16; i++)
    {
      if ((this->state >> i) & 1)
      {
        this->onDuration[i] += currentMillis - previousMillis;
      }
      else
      {
        this->onDuration[i] = 0;
      }
    }

    previousMillis = currentMillis;
  }
}

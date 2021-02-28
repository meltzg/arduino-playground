#include <Arduino.h>
#include <Components.h>

const __int24 RAINBOW[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE};

const uint16_t getCharacter(char character)
{
  switch (character)
  {
#ifdef __SEG_16__
  case '!':
    return 0x000C; /* ! */
  case '"':
    return 0x0204; /* " */
  case '#':
    return 0xAA3C; /* # */
  case '$':
    return 0xAABB; /* $ */
  case '%':
    return 0xEE99; /* % */
  case '&':
    return 0x9371; /* & */
  case '\'':
    return 0x0200; /* ' */
  case '(':
    return 0x1400; /* ( */
  case ')':
    return 0x4100; /* ) */
  case '*':
    return 0xFF00; /* * */
  case '+':
    return 0xAA00; /* + */
  case ',':
    return 0x4000; /* ; */
  case '-':
    return 0x8800; /* - */
  case '.':
    return 0x1000; /* . */
  case '/':
    return 0x4400; /* / */
  case '0':
    return 0x44FF; /* 0 */
  case '1':
    return 0x040C; /* 1 */
  case '2':
    return 0x8877; /* 2 */
  case '3':
    return 0x083F; /* 3 */
  case '4':
    return 0x888C; /* 4 */
  case '5':
    return 0x90B3; /* 5 */
  case '6':
    return 0x88FB; /* 6 */
  case '7':
    return 0x000F; /* 7 */
  case '8':
    return 0x88FF; /* 8 */
  case '9':
    return 0x88BF; /* 9 */
  case ':':
    return 0x2200; /* : */
  case ';':
    return 0x4200; /* ; */
  case '<':
    return 0x9400; /* < */
  case '=':
    return 0x8830; /* = */
  case '>':
    return 0x4900; /* > */
  case '?':
    return 0x2807; /* ? */
  case '@':
    return 0x0AF7; /* @ */
  case 'A':
    return 0x88CF; /* A */
  case 'B':
    return 0x2A3F; /* B */
  case 'C':
    return 0x00F3; /* C */
  case 'D':
    return 0x223F; /* D */
  case 'E':
    return 0x80F3; /* E */
  case 'F':
    return 0x80C3; /* F */
  case 'G':
    return 0x08FB; /* G */
  case 'H':
    return 0x88CC; /* H */
  case 'I':
    return 0x2233; /* I */
  case 'J':
    return 0x007C; /* J */
  case 'K':
    return 0x94C0; /* K */
  case 'L':
    return 0x00F0; /* L */
  case 'M':
    return 0x05CC; /* M */
  case 'N':
    return 0x11CC; /* N */
  case 'O':
    return 0x00FF; /* O */
  case 'P':
    return 0x88C7; /* P */
  case 'Q':
    return 0x10FF; /* Q */
  case 'R':
    return 0x98C7; /* R */
  case 'S':
    return 0x88BB; /* S */
  case 'T':
    return 0x2203; /* T */
  case 'U':
    return 0x00FC; /* U */
  case 'V':
    return 0x44C0; /* V */
  case 'W':
    return 0x50CC; /* W */
  case 'X':
    return 0x5500; /* X */
  case 'Y':
    return 0x88BC; /* Y */
  case 'Z':
    return 0x4433; /* Z */
  case '[':
    return 0x2212; /* [ */
  case ']':
    return 0x1100; /* \ */
  case '\\':
    return 0x2221; /* ] */
  case '^':
    return 0x5000; /* ^ */
  case '_':
    return 0x0030; /* _ */
  case '`':
    return 0x0100; /* ` */
  case 'a':
    return 0xA070; /* a */
  case 'b':
    return 0xA0E0; /* b */
  case 'c':
    return 0x8060; /* c */
  case 'd':
    return 0x281C; /* d */
  case 'e':
    return 0xC060; /* e */
  case 'f':
    return 0xAA02; /* f */
  case 'g':
    return 0xA2A1; /* g */
  case 'h':
    return 0xA0C0; /* h */
  case 'i':
    return 0x2000; /* i */
  case 'j':
    return 0x2260; /* j */
  case 'k':
    return 0x3600; /* k */
  case 'l':
    return 0x00C0; /* l */
  case 'm':
    return 0xA848; /* m */
  case 'n':
    return 0xA040; /* n */
  case 'o':
    return 0xA060; /* o */
  case 'p':
    return 0x82C1; /* p */
  case 'q':
    return 0xA281; /* q */
  case 'r':
    return 0x8040; /* r */
  case 's':
    return 0xA0A1; /* s */
  case 't':
    return 0x80E0; /* t */
  case 'u':
    return 0x2060; /* u */
  case 'v':
    return 0x4040; /* v */
  case 'w':
    return 0x5048; /* w */
  case 'x':
    return 0x5500; /* x */
  case 'y':
    return 0x0A1C; /* y */
  case 'z':
    return 0xC020; /* z */
  case '{':
    return 0xA212; /* { */
  case '|':
    return 0x2200; /* | */
  case '}':
    return 0x2A21; /* } */
  case '~':
    return 0xCC00; /* ~ */
  default:
    return 0x0000; /* space */
#else
  case '!':
    return 0x4006; /* ! */
  case '"':
    return 0x0202; /* " */
  case '#':
    return 0x12CE; /* # */
  case '$':
    return 0x12ED; /* $ */
  case '%':
    return 0x3FE4; /* % */
  case '&':
    return 0x2359; /* & */
  case '\'':
    return 0x0200; /* ' */
  case '(':
    return 0x2400; /* ( */
  case ')':
    return 0x0900; /* ) */
  case '*':
    return 0x3FC0; /* * */
  case '+':
    return 0x12C0; /* + */
  case ',':
    return 0x0800; /* , */
  case '-':
    return 0x00C0; /* - */
  case '.':
    return 0x4000; /* . */
  case '/':
    return 0x0C00; /* / */
  case '0':
    return 0x0C3F; /* 0 */
  case '1':
    return 0x0406; /* 1 */
  case '2':
    return 0x00DB; /* 2 */
  case '3':
    return 0x008F; /* 3 */
  case '4':
    return 0x00E6; /* 4 */
  case '5':
    return 0x2069; /* 5 */
  case '6':
    return 0x00FD; /* 6 */
  case '7':
    return 0x0007; /* 7 */
  case '8':
    return 0x00FF; /* 8 */
  case '9':
    return 0x00EF; /* 9 */
  case ':':
    return 0x1200; /* : */
  case ';':
    return 0x0A00; /* ; */
  case '<':
    return 0x2440; /* < */
  case '=':
    return 0x00C8; /* = */
  case '>':
    return 0x0980; /* > */
  case '?':
    return 0x5083; /* ? */
  case '@':
    return 0x02BB; /* @ */
  case 'A':
    return 0x00F7; /* A */
  case 'B':
    return 0x128F; /* B */
  case 'C':
    return 0x0039; /* C */
  case 'D':
    return 0x120F; /* D */
  case 'E':
    return 0x0079; /* E */
  case 'F':
    return 0x0071; /* F */
  case 'G':
    return 0x00BD; /* G */
  case 'H':
    return 0x00F6; /* H */
  case 'I':
    return 0x1209; /* I */
  case 'J':
    return 0x001E; /* J */
  case 'K':
    return 0x2470; /* K */
  case 'L':
    return 0x0038; /* L */
  case 'M':
    return 0x0536; /* M */
  case 'N':
    return 0x2136; /* N */
  case 'O':
    return 0x003F; /* O */
  case 'P':
    return 0x00F3; /* P */
  case 'Q':
    return 0x203F; /* Q */
  case 'R':
    return 0x20F3; /* R */
  case 'S':
    return 0x00ED; /* S */
  case 'T':
    return 0x1201; /* T */
  case 'U':
    return 0x003E; /* U */
  case 'V':
    return 0x0C30; /* V */
  case 'W':
    return 0x2836; /* W */
  case 'X':
    return 0x2D00; /* X */
  case 'Y':
    return 0x00EE; /* Y */
  case 'Z':
    return 0x0C09; /* Z */
  case '[':
    return 0x0039; /* [ */
  case '\\':
    return 0x2100; /* \ */
  case ']':
    return 0x000F; /* ] */
  case '^':
    return 0x2800; /* ^ */
  case '_':
    return 0x0008; /* _ */
  case '`':
    return 0x0100; /* ` */
  case 'a':
    return 0x1058; /* a */
  case 'b':
    return 0x2078; /* b */
  case 'c':
    return 0x00D8; /* c */
  case 'd':
    return 0x088E; /* d */
  case 'e':
    return 0x0858; /* e */
  case 'f':
    return 0x14C0; /* f */
  case 'g':
    return 0x048E; /* g */
  case 'h':
    return 0x1070; /* h */
  case 'i':
    return 0x1000; /* i */
  case 'j':
    return 0x0A10; /* j */
  case 'k':
    return 0x3600; /* k */
  case 'l':
    return 0x0030; /* l */
  case 'm':
    return 0x10D4; /* m */
  case 'n':
    return 0x1050; /* n */
  case 'o':
    return 0x00DC; /* o */
  case 'p':
    return 0x0170; /* p */
  case 'q':
    return 0x0486; /* q */
  case 'r':
    return 0x0050; /* r */
  case 's':
    return 0x2088; /* s */
  case 't':
    return 0x0078; /* t */
  case 'u':
    return 0x001C; /* u */
  case 'v':
    return 0x0810; /* v */
  case 'w':
    return 0x2814; /* w */
  case 'x':
    return 0x2D00; /* x */
  case 'y':
    return 0x028E; /* y */
  case 'z':
    return 0x0848; /* z */
  case '{':
    return 0x0949; /* { */
  case '|':
    return 0x1200; /* | */
  case '}':
    return 0x2489; /* } */
  case '~':
    return 0x0CC0; /* ~ */
  default:
    return 0x0000; /* (space) */
#endif
  }
}

SegmentDisplay::SegmentDisplay(int latchPin, int clockPin, int dataPin, int leftCommonPin, int rightCommonPin) : latchPin(latchPin), clockPin(clockPin), dataPin(dataPin), leftCommonPin(leftCommonPin), rightCommonPin(rightCommonPin)
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(leftCommonPin, OUTPUT);
  pinMode(rightCommonPin, OUTPUT);
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
  this->chars[0] = chars[0];
  this->chars[1] = chars[1];
}

void SegmentDisplay::render(unsigned long currentMillis)
{
  if (currentMillis - previousMillis >= DIGIT_SWITCH_DELAY)
  {
    showLeft = !showLeft;
    previousMillis = currentMillis;

    // Assume common anode. to turn just left on, right must be high, left must be low
    if (showLeft)
    {
      digitalWrite(rightCommonPin, HIGH);
      registerWrite(getCharacter(chars[0]));
      digitalWrite(leftCommonPin, LOW);
    }
    else
    {
      digitalWrite(leftCommonPin, HIGH);
      registerWrite(getCharacter(chars[1]));
      digitalWrite(rightCommonPin, LOW);
    }
  }
}

LEDStatusDisplay::LEDStatusDisplay(int dataPin, int numLeds) : dataPin(dataPin), numLeds(numLeds)
{
  pixels = Adafruit_NeoPixel(numLeds, dataPin, NEO_GRB + NEO_KHZ800);

  pixels.begin();
}

void LEDStatusDisplay::render(unsigned long currentMillis)
{
  if (currentMillis - previousMillis >= LED_UPDATE_DELAY)
  {
    previousMillis = currentMillis;
    pixels.clear();
    pixels.setBrightness(brightness);
    for (int i = 0; i < numLeds; i++)
    {
      pixels.setPixelColor(i, grbs[i]);
    }
    pixels.show();
  }
}

void LEDStatusDisplay::setState(__int24 *grbs, byte brightness)
{
  this->brightness = brightness;
  this->grbs = grbs;
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

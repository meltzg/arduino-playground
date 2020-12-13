#import <Arduino.h>
#import "components.h"

const uint16_t getCharacter(char character)
{
  switch (character)
  {
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


void SegmentDisplay::setChars(char *chars) {
  this->chars[0] = chars[0];
  this->chars[1] = chars[1];
}

void SegmentDisplay::render(unsigned long currentMillis) {
  if (currentMillis - previousMillis >= digitSwitchDelay) {
    showLeft = !showLeft;
    previousMillis = currentMillis;

    // Assume common anode. to turn just left on, right must be high, left must be low
    if (showLeft) {
      digitalWrite(rightCommonPin, HIGH);
      registerWrite(getCharacter(chars[0]));
      digitalWrite(leftCommonPin, LOW);
    } else {

      digitalWrite(leftCommonPin, HIGH);
      registerWrite(getCharacter(chars[1]));
      digitalWrite(rightCommonPin, LOW);
    }
  }
}

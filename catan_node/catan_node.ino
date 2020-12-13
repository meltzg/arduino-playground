#import "components.h"

#define SEGMENT_LATCH 2
#define SEGMENT_CLOCK 3
#define SEGMENT_DATA 4
#define SEGMENT_LEFT 5
#define SEGMENT_RIGHT 6

SegmentDisplay tileValue(
  SEGMENT_LATCH,
  SEGMENT_CLOCK,
  SEGMENT_DATA,
  SEGMENT_LEFT,
  SEGMENT_RIGHT
);

void setup()
{
  pinMode(SEGMENT_LATCH, OUTPUT);
  pinMode(SEGMENT_CLOCK, OUTPUT);
  pinMode(SEGMENT_DATA, OUTPUT);
  pinMode(SEGMENT_LEFT, OUTPUT);
  pinMode(SEGMENT_RIGHT, OUTPUT);
  Serial.begin(9600);

  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect)
{
  unsigned long currentMillis = millis();
  tileValue.render(currentMillis);
}

void loop()
{
  const char *message = "  Hello World!  ";
  for (int i = 0; i < strlen(message) - 1; i++) {
    char buf[3] = {0};
    sprintf(buf, "%02x", i);
    Serial.println(buf);

    tileValue.setChars(message + i);

    delay(500);
  }
}

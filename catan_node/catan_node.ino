#import "components.h"

/*
   Edges
   1 /\ 2
   0 | | 3
   5 \/ 4

   Settlements
      1
   0 /\ 2
     | |
   5 \/ 3
     4
*/

#define SEGMENT_LATCH 2
#define SEGMENT_CLOCK 3
#define SEGMENT_DATA 4
#define SEGMENT_LEFT 5
#define SEGMENT_RIGHT 6

#define LED_ARRAY 9

#define BTN_LOAD A0
#define BTN_CLK_ENABLE A1
#define BTN_DATA A2
#define BTN_CLOCK A3

#define NUM_LEDS 13


SegmentDisplay tileValue(
  SEGMENT_LATCH,
  SEGMENT_CLOCK,
  SEGMENT_DATA,
  SEGMENT_LEFT,
  SEGMENT_RIGHT
);

LEDStatusDisplay tileStateDisplay(LED_ARRAY, NUM_LEDS);

ButtonArray16 interface(
  BTN_LOAD,
  BTN_CLK_ENABLE,
  BTN_DATA,
  BTN_CLOCK
);

void setup()
{
  Serial.begin(9600);

  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect)
{
  unsigned long currentMillis = millis();
  tileValue.render(currentMillis);
  tileStateDisplay.render(currentMillis);
  interface.render(currentMillis);
}

void loop()
{
  char buf[3] = {0};
  uint16_t state = interface.getState();
  sprintf(buf, "%02x", state);

  tileValue.setChars(buf);
}

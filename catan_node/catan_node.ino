#import "components.h"

/*
   Edges         LEDs
   1 /\ 2        3 /\ 6
   0 | | 3       0 | | 9
   5 \/ 4        11 \/ 10

   Settlements   LEDs
      1              4,5
   0 /\ 2        1,2 /\ 7,8
     | |
   5 \/ 3
     4

    Land
     /\
    |12|
     \/
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
#define NUM_BUTTONS 10

#define NUM_ROADS 6
#define NUM_SETTLEMENTS 3

#define DESERT 0xB4D28C

const __int24 PLAYER_COLORS[] = { RED, ORANGE, GREEN, BLUE, PURPLE, WHITE };

const byte ROAD_POSITIONS[] = {0, 3, 6, 9, 10, 11};
const byte SETTLEMENT_POSITIONS[][2] = {{1, 2}, {4, 5}, {7, 8}};
#define LAND_POSITION 12


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

__int24 borderColors[NUM_LEDS] = { BLACK };

byte roadOwners[NUM_ROADS] = { 0 };
byte settlementOwners[NUM_SETTLEMENTS] = { 0 };
bool isCity[NUM_SETTLEMENTS] = { false };
__int24 landType = DESERT;

byte currentPlayer = 0;

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
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if ((state >> i) & 1) {
      borderColors[i] = PLAYER_COLORS[currentPlayer];
    } else {
      borderColors[i] = 0;
    }
  }
  sprintf(buf, "%02x", state);

  tileValue.setChars(buf);
  tileStateDisplay.setState(borderColors, 50);
}

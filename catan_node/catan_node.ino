#import "components.h"

/*
   Edges         LEDs        Buttons
   1 /\ 2        3 /\ 6     2 /\ 4
   0 | | 3       0 | | 9    0 | | 6
   5 \/ 4        11\/ 10    8 \/ 7

   Settlements   LEDs        Buttons
      1              4,5       3
   0 /\ 2        1,2 /\ 7,8  1 /\ 5
     | |
   5 \/ 3
     4

    Land         LEDs        Buttons
     /\           /\          /\
    | X|         |12|        | 9|
     \/           \/          \/
*/

#define SEGMENT_LATCH 2
#define SEGMENT_CLOCK 3
#define SEGMENT_DATA 4
#define SEGMENT_LEFT 5
#define SEGMENT_RIGHT 6

#define LED_ARRAY 9
#define BRIGHTNESS_STEP 32
#define MIN_BRIGHTNESS 5

#define BTN_LOAD A0
#define BTN_CLK_ENABLE A1
#define BTN_DATA A2
#define BTN_CLOCK A3

#define SEED_PIN A5

#define NUM_LEDS 13
#define NUM_BUTTONS 10

#define NUM_ROADS 6
#define NUM_SETTLEMENTS 3

#define DESERT 0xB4D28C
#define BRICK 0x41CB54
#define SHEEP 0xFCB038
#define WOOD 0xAC0313
#define STONE 0xED3D97
#define WHEAT YELLOW

const __int24 PLAYER_COLORS[] = { RED, ORANGE, GREEN, BLUE, PURPLE, WHITE };

const byte ROAD_LED_POS[] = { 0, 3, 6, 9, 10, 11 };
const byte ROAD_BTN_POS[] = { 0, 2, 4, 6, 7, 8 };
const byte SETTLEMENT_LED_POS[][2] = { {1, 2}, {4, 5}, {7, 8} };
const byte SETTLEMENT_BTN_POS[] = { 1, 3, 5 };
#define LAND_LED_POS 12
#define LAND_BTN_POS 9
#define BRIGHTNESS_BTN 10


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
byte brightness = 64;

byte roadOwners[NUM_ROADS] = { 0 };
byte settlementOwners[NUM_SETTLEMENTS] = { 0 };
bool isCity[NUM_SETTLEMENTS] = { false };
__int24 landType = WOOD;
byte rollValue = 0;
bool hasRobber = false;

bool playerSelectMode;

byte currentPlayer = 0;
uint16_t previousState = 0;

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(SEED_PIN));

  rollValue = 7;
  while (rollValue == 7) {
    rollValue = random(1, 7) + random(1, 7);
  }
  char buf[3] = {0};
  sprintf(buf, "%02d", rollValue);
  tileValue.setChars(buf);

  borderColors[LAND_LED_POS] = landType;

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
  uint16_t state = interface.getState();
  if (state != previousState) {
    updateRoads(state);
    updateSettlements(state);
    updateRobber(state);

    if (((previousState >> BRIGHTNESS_BTN) & 1) && ((state >> BRIGHTNESS_BTN) & 1) == 0) {
      brightness += BRIGHTNESS_STEP;
    }
  }
  tileStateDisplay.setState(borderColors, max(brightness, MIN_BRIGHTNESS));
  previousState = state;
}

void updateRoads(uint16_t newBtnState) {
  for (int i = 0; i < NUM_ROADS; i++) {
    byte ledPos = ROAD_LED_POS[i];
    byte btnPos = ROAD_BTN_POS[i];

    if (((previousState >> btnPos) & 1) && ((newBtnState >> btnPos) & 1) == 0) {
      if (borderColors[ledPos] == BLACK) {
        borderColors[ledPos] = PLAYER_COLORS[currentPlayer];
      } else if (borderColors[ledPos] == PLAYER_COLORS[currentPlayer]) {
        borderColors[ledPos] = BLACK;
      }
    }
  }
}

void updateSettlements(uint16_t newBtnState) {
  for (int i = 0; i < NUM_SETTLEMENTS; i++) {
    byte *ledPos = SETTLEMENT_LED_POS[i];
    byte btnPos = SETTLEMENT_BTN_POS[i];

    if (((previousState >> btnPos) & 1) && ((newBtnState >> btnPos) & 1) == 0) {
      if (borderColors[ledPos[0]] == BLACK) {
        borderColors[ledPos[0]] = PLAYER_COLORS[currentPlayer];
      } else if (borderColors[ledPos[0]] == PLAYER_COLORS[currentPlayer]) {
        if  (!isCity[i]) {
          borderColors[ledPos[1]] = PLAYER_COLORS[currentPlayer];
          isCity[i] = true;
        } else {
          borderColors[ledPos[0]] = BLACK;
          borderColors[ledPos[1]] = BLACK;
          isCity[i] = false;
        }
      }
    }
  }
}

void updateRobber(uint16_t newBtnState) {
  char buf[3] = {0};
  if (((previousState >> LAND_BTN_POS) & 1) && ((newBtnState >> LAND_BTN_POS) & 1) == 0) {
    if (hasRobber) {
      hasRobber = false;
      sprintf(buf, "%02d", rollValue);
      tileValue.setChars(buf);
    } else {
      hasRobber = true;
      tileValue.setChars("Rb");
    }
  }
}

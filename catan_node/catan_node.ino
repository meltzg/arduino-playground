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

#define SEGMENT_LATCH A0
#define SEGMENT_CLOCK A1
#define SEGMENT_DATA A2
#define SEGMENT_LEFT A3
#define SEGMENT_RIGHT A4

#define LED_ARRAY 9
#define BRIGHTNESS_STEP 32
#define MIN_BRIGHTNESS 5

#define BTN_LOAD 10
#define BTN_CLK_ENABLE 11
#define BTN_DATA 12
#define BTN_CLOCK 13

#define SEED_PIN A5

#define NUM_LEDS 13
#define NUM_BUTTONS 10

#define PLAYER_SELECT_DELAY 500

#define NUM_ROADS 6
#define NUM_SETTLEMENTS 3
#define UNOWNED -1

#define DESERT 0xB4D28C
#define BRICK 0x41CB54
#define SHEEP 0xFCB038
#define WOOD 0xAC0313
#define STONE 0xED3D97
#define WHEAT YELLOW

const __int24 PLAYER_COLORS[] = { RED, ORANGE, GREEN, BLUE, PURPLE, WHITE };
const __int24 LAND_COLORS[] = { DESERT, BRICK, SHEEP, WOOD, STONE, WHEAT };

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

short roadOwners[NUM_ROADS];
short settlementOwners[NUM_SETTLEMENTS];
bool isCity[NUM_SETTLEMENTS] = { false };
__int24 landType = WOOD;
byte rollValue = 0;
bool hasRobber = false;

bool playerSelectMode = false;

byte currentPlayer = 0;
uint16_t previousState = 0;

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(SEED_PIN));

  for (int i = 0; i < NUM_ROADS; i++) {
    roadOwners[i] = UNOWNED;
  }
  for (int i = 0; i < NUM_SETTLEMENTS; i++) {
    settlementOwners[i] = UNOWNED;
  }

  rollValue = 7;
  while (rollValue == 7) {
    rollValue = random(1, 7) + random(1, 7);
  }
  char buf[3] = {0};
  sprintf(buf, "%02d", rollValue);
  tileValue.setChars(buf);

  landType = LAND_COLORS[random(0, 6)];
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
  bool skipRobber = false;

  uint16_t state = interface.getState();
  if (!playerSelectMode && interface.getOnDuration(LAND_BTN_POS) >= PLAYER_SELECT_DELAY) {
    playerSelectMode = true;
    updateCurrentPlayer(state);
  } else if (playerSelectMode && interface.getOnDuration(LAND_BTN_POS) == 0) {
    playerSelectMode = false;
    skipRobber = true;
    borderColors[LAND_LED_POS] = landType;
  }
  if (state != previousState) {
    if (!playerSelectMode) {
      updateRoads(state);
      updateSettlements(state);
      if (!skipRobber) {
        updateRobber(state);
      }
    } else {
      updateCurrentPlayer(state);
    }

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
      if (roadOwners[i] == UNOWNED) {
        roadOwners[i] = currentPlayer;
      } else if (roadOwners[i] == currentPlayer) {
        roadOwners[i] = UNOWNED;
      }
    }

    if (roadOwners[i] == UNOWNED) {
      borderColors[ledPos] = BLACK;
    } else {
      borderColors[ledPos] = PLAYER_COLORS[roadOwners[i]];
    }

  }
}

void updateSettlements(uint16_t newBtnState) {
  for (int i = 0; i < NUM_SETTLEMENTS; i++) {
    byte *ledPos = SETTLEMENT_LED_POS[i];
    byte btnPos = SETTLEMENT_BTN_POS[i];

    if (((previousState >> btnPos) & 1) && ((newBtnState >> btnPos) & 1) == 0) {
      if (settlementOwners[i] == UNOWNED) {
        settlementOwners[i] = currentPlayer;
      } else if (settlementOwners[i] == currentPlayer) {
        if  (!isCity[i]) {
          isCity[i] = true;
        } else {
          isCity[i] = false;
          settlementOwners[i] = UNOWNED;
        }
      }
    }

    if (settlementOwners[i] == UNOWNED) {
      borderColors[ledPos[0]] = BLACK;
      borderColors[ledPos[1]] = BLACK;
    } else {
      borderColors[ledPos[0]] = PLAYER_COLORS[settlementOwners[i]];
      if (isCity[i]) {
        borderColors[ledPos[1]] = PLAYER_COLORS[settlementOwners[i]];
      }
    }

    char buf[50] = {0};
    sprintf(buf, "Settlement %d owner: %d", i, settlementOwners[i]);
    Serial.println(buf);
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

void updateCurrentPlayer(uint16_t newBtnState) {
  for (int i = 0; i < NUM_ROADS; i++) {
    byte btnPos = ROAD_BTN_POS[i];

    if (((previousState >> btnPos) & 1) && ((newBtnState >> btnPos) & 1) == 0) {
      currentPlayer = i;
      break;
    }
  }
  for (int i = 0; i < NUM_ROADS; i++) {
    byte ledPos = ROAD_LED_POS[i];
    borderColors[ledPos] = PLAYER_COLORS[i];
  }
  for (int i = 0; i < NUM_SETTLEMENTS; i++) {
    byte *ledPos = SETTLEMENT_LED_POS[i];
    borderColors[ledPos[0]] = BLACK;
    borderColors[ledPos[1]] = BLACK;
  }
  borderColors[LAND_LED_POS] = PLAYER_COLORS[currentPlayer];
}

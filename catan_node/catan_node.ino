#include <SoftwareSerial.h>
#include <CommonMessaging.h>
#include <Components.h>

/*
   Edges         LEDs        Buttons
   1 /\ 2        3 /\ 6     2 /\ 4
   0 | | 3       0 | | 7    0 | | 5
   5 \/ 4        9 \/ 8     7 \/ 6

   Settlements   LEDs        Buttons
      1              4,5       3
   0 /\ 2        1,2 /       1 /
     | |
   5 \/ 3
     4

    Land         LEDs        Buttons
     /\           /\          /\
    | X|         |10|        | 8|
     \/           \/          \/
*/

#define SEGMENT_LATCH A0
#define SEGMENT_CLOCK A1
#define SEGMENT_DATA A2
#define SEGMENT_LEFT A3
#define SEGMENT_RIGHT A4

#define LED_ARRAY 7
#define BRIGHTNESS_STEP 32
#define MIN_BRIGHTNESS 5

#define BTN_LOAD 10
#define BTN_CLK_ENABLE 11
#define BTN_DATA 12
#define BTN_CLOCK 13

#define SEED_PIN A5

#define NUM_LEDS 11
#define NUM_BUTTONS 10

#define PLAYER_SELECT_DELAY 500

#define NUM_ROADS 6
#define NUM_SETTLEMENTS 2
#define UNOWNED -1

#define NUM_DICE 2
#define DIE_SIDES 6
#define MAX_ROLL NUM_DICE *DIE_SIDES
#define MIN_ROLL NUM_DICE

#define OCEAN 0x3813BE
#define DESERT 0xB4D28C
#define BRICK 0x41CB54
#define SHEEP 0xFCB038
#define WOOD 0xAC0313
#define STONE 0xED3D97
#define WHEAT YELLOW

const __int24 PLAYER_COLORS[] = {RED, ORANGE, GREEN, BLUE, PURPLE, WHITE};
const __int24 LAND_COLORS[] = {OCEAN, DESERT, BRICK, SHEEP, WOOD, STONE, WHEAT, OCEAN};

const byte ROAD_LED_POS[] = {0, 3, 6, 7, 8, 9};
const byte ROAD_BTN_POS[] = {0, 2, 4, 5, 6, 7};
const byte SETTLEMENT_LED_POS[][2] = {{1, 2}, {4, 5}};
const byte SETTLEMENT_BTN_POS[] = {1, 3};
#define LAND_LED_POS 10
#define LAND_BTN_POS 8
#define BRIGHTNESS_BTN 9

SegmentDisplay tileValue(
    SEGMENT_LATCH,
    SEGMENT_CLOCK,
    SEGMENT_DATA,
    SEGMENT_LEFT,
    SEGMENT_RIGHT,
    300);

LEDStatusDisplay tileStateDisplay(LED_ARRAY, NUM_LEDS);

ButtonArray16 interface(
    BTN_LOAD,
    BTN_CLK_ENABLE,
    BTN_DATA,
    BTN_CLOCK);

SoftwareSerial netPort(8, 9);
NodeId_t netId = 0;

__int24 borderColors[NUM_LEDS] = {BLACK};
byte brightness = 64;

short roadOwners[NUM_ROADS];
short settlementOwners[NUM_SETTLEMENTS];
bool isCity[NUM_SETTLEMENTS] = {false};
__int24 landType = OCEAN;
byte rollValue = 0;
bool hasRobber = false;

bool playerSelectMode = false;

byte currentPlayer = 0;
uint16_t previousState = 0;

boolean playStarted = true;

void setup()
{
  Serial.begin(9600);
  netPort.begin(SOFT_BAUD);
  randomSeed(analogRead(SEED_PIN));

  for (int i = 0; i < NUM_ROADS; i++)
  {
    roadOwners[i] = UNOWNED;
  }
  for (int i = 0; i < NUM_SETTLEMENTS; i++)
  {
    settlementOwners[i] = UNOWNED;
  }

  setTileValue(rollValue);
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
  if (playStarted && !playerSelectMode && interface.getOnDuration(LAND_BTN_POS) >= PLAYER_SELECT_DELAY)
  {
    playerSelectMode = true;
    updateCurrentPlayer(state);
  }
  else if (playerSelectMode && interface.getOnDuration(LAND_BTN_POS) == 0)
  {
    playerSelectMode = false;
    skipRobber = true;
    borderColors[LAND_LED_POS] = landType;
  }
  if (state != previousState)
  {
    if (!playStarted)
    {
      setupGame(state);
    }
    else if (!playerSelectMode)
    {
      updateRoads(state);
      updateSettlements(state);
      if (!skipRobber)
      {
        updateRobber(state);
      }
    }
    else
    {
      updateCurrentPlayer(state);
    }

    if (((previousState >> BRIGHTNESS_BTN) & 1) && ((state >> BRIGHTNESS_BTN) & 1) == 0)
    {
      brightness += BRIGHTNESS_STEP;
    }
  }
  tileStateDisplay.setState(borderColors, max(brightness, MIN_BRIGHTNESS));
  previousState = state;
}

void updateRoads(uint16_t newBtnState)
{
  for (int i = 0; i < NUM_ROADS; i++)
  {
    byte ledPos = ROAD_LED_POS[i];
    byte btnPos = ROAD_BTN_POS[i];

    if (((previousState >> btnPos) & 1) && ((newBtnState >> btnPos) & 1) == 0)
    {
      if (roadOwners[i] == UNOWNED)
      {
        roadOwners[i] = currentPlayer;
      }
      else if (roadOwners[i] == currentPlayer)
      {
        roadOwners[i] = UNOWNED;
      }
    }

    if (roadOwners[i] == UNOWNED)
    {
      borderColors[ledPos] = BLACK;
    }
    else
    {
      borderColors[ledPos] = PLAYER_COLORS[roadOwners[i]];
    }
  }
}

void updateSettlements(uint16_t newBtnState)
{
  for (int i = 0; i < NUM_SETTLEMENTS; i++)
  {
    byte *ledPos = SETTLEMENT_LED_POS[i];
    byte btnPos = SETTLEMENT_BTN_POS[i];

    if (((previousState >> btnPos) & 1) && ((newBtnState >> btnPos) & 1) == 0)
    {
      if (settlementOwners[i] == UNOWNED)
      {
        settlementOwners[i] = currentPlayer;
      }
      else if (settlementOwners[i] == currentPlayer)
      {
        if (!isCity[i])
        {
          isCity[i] = true;
        }
        else
        {
          isCity[i] = false;
          settlementOwners[i] = UNOWNED;
        }
      }
    }

    if (settlementOwners[i] == UNOWNED)
    {
      borderColors[ledPos[0]] = BLACK;
      borderColors[ledPos[1]] = BLACK;
    }
    else
    {
      borderColors[ledPos[0]] = PLAYER_COLORS[settlementOwners[i]];
      if (isCity[i])
      {
        borderColors[ledPos[1]] = PLAYER_COLORS[settlementOwners[i]];
      }
    }

    char buf[50] = {0};
    sprintf(buf, "Settlement %d owner: %d", i, settlementOwners[i]);
    Serial.println(buf);
  }
}

void updateRobber(uint16_t newBtnState)
{
  char buf[3] = {0};
  if (((previousState >> LAND_BTN_POS) & 1) && ((newBtnState >> LAND_BTN_POS) & 1) == 0)
  {
    if (hasRobber)
    {
      hasRobber = false;
      setTileValue(rollValue);
    }
    else
    {
      hasRobber = true;
      setTileValue(0xFF);
    }
  }
}

void updateCurrentPlayer(uint16_t newBtnState)
{
  for (int i = 0; i < NUM_ROADS; i++)
  {
    byte btnPos = ROAD_BTN_POS[i];

    if (((previousState >> btnPos) & 1) && ((newBtnState >> btnPos) & 1) == 0)
    {
      currentPlayer = i;
      break;
    }
  }
  for (int i = 0; i < NUM_ROADS; i++)
  {
    byte ledPos = ROAD_LED_POS[i];
    borderColors[ledPos] = PLAYER_COLORS[i];
  }
  for (int i = 0; i < NUM_SETTLEMENTS; i++)
  {
    byte *ledPos = SETTLEMENT_LED_POS[i];
    borderColors[ledPos[0]] = BLACK;
    borderColors[ledPos[1]] = BLACK;
  }
  borderColors[LAND_LED_POS] = PLAYER_COLORS[currentPlayer];
}

void setupGame(uint16_t newBtnState)
{
  if (((previousState >> LAND_BTN_POS) & 1) == 0 && ((newBtnState >> LAND_BTN_POS) & 1))
  {
    return;
  }
  int maxRetries = 100;
  Message idRequest;
  idRequest.source = EMPTY;
  idRequest.dest = EMPTY;
  idRequest.payloadSize = 0;
  idRequest.sysCommand = ROUTER_GET_ID;
  idRequest.body = NULL;

  if (ackWait(&netPort, maxRetries))
  {
    writeMessage(&netPort, idRequest);
    netPort.readBytes((byte *)&netId, sizeof(NodeId_t));
  }
  else
  {
    Serial.println("Failure");
    return;
  }

  Serial.print("My ID: ");
  Serial.println(netId, HEX);

  playStarted = true;
}

void setTileValue(byte val)
{
  if (val < MIN_ROLL)
  {
    tileValue.setChars("  ");
  }
  else if (val > MAX_ROLL)
  {
    tileValue.setChars("Robber");
  }
  else
  {
    char buf[3] = {0};
    sprintf(buf, "%02d", val);
    tileValue.setChars(buf);
  }
}

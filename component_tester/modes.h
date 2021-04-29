#ifndef _MODES_H_
#define _MODES_H_

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

// Mode Specific Defines
// Netowrk test
#define BTN_ID 9
#define BTN_NEIGHBORS 1
#define BTN_DISCOVER 10

// Catan
#define LED_LAND 10
#define BTN_LAND 9
#define SEED_PIN A5

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

class Mode
{
protected:
    static const byte EDGE_LED_POS[6];
    static const byte EDGE_BTN_POS[6];
    static const byte CORNER_LED_POS[2][2];
    static const byte CORNER_BTN_POS[2];

    const SegmentDisplay &disp;
    const LEDStatusDisplay &leds;
    const ButtonArray16 &btns;
    const SoftwareSerial &netPort;

    unsigned long previousMillis = 0;
    uint16_t previousState = 0;

public:
    Mode(const SegmentDisplay &disp, const LEDStatusDisplay &leds, const ButtonArray16 &btns, const SoftwareSerial &netPort) : disp(disp), leds(leds), btns(btns), netPort(netPort) { init(); }
    virtual void init() {}
    virtual void process(unsigned long currentMillis){};
};

class ComponentTestMode : public Mode
{
public:
    using Mode::Mode;

    virtual void init();
    virtual void process(unsigned long currentMillis);
};

class NetworkTestMode : public Mode
{
public:
    using Mode::Mode;

    virtual void init();
    virtual void process(unsigned long currentMillis);

private:
    NodeId_t myId = EMPTY;
    NodeId_t neighborIds[6];
    char displayMessage[100] = {0};

    void processMessage(Stream *srcPort, const Message &message);
    void handleNodeResponse(const Message &message);
    void handleIdRequest();
    void handleNeighborRequest(NodeId_t destination);
    void handleDiscoveryRequest() {}
};

class CatanMode : public Mode
{
public:
    using Mode::Mode;

    virtual void init();
    virtual void process(unsigned long currentMillis);

private:
    static const __int24 PLAYER_COLORS[6];
    static const __int24 LAND_COLORS[8];

    NodeId_t myId = EMPTY;
    NodeId_t neighborIds[6];
    __int24 *borderColors = NULL;
    short roadOwners[NUM_ROADS];
    short settlementOwners[NUM_SETTLEMENTS];
    bool isCity[NUM_SETTLEMENTS] = {false};
    __int24 landType = BLACK;
    byte rollValue = 0;
    bool hasRobber = false;
    bool playerSelectMode = false;
    byte currentPlayer = 3;
    boolean playStarted = false;

    void updateRoads(uint16_t state);
    void updateSettlements(uint16_t state);
    void updateRobber(uint16_t state);
    void updateCurrentPlayer(uint16_t state);
    void setupGame();
    void setTileValue(byte val);
};

#endif // _MODES_H_

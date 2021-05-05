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

#define CATAN_SETUP_NEIGHBORS 1

#define PLAYER_SELECT_DELAY 500

#define NUM_ROADS 6
#define NUM_SETTLEMENTS 2
#define UNOWNED -1

#define NUM_DICE 2
#define DIE_SIDES 6
#define MAX_ROLL NUM_DICE *DIE_SIDES
#define MIN_ROLL NUM_DICE

#define OCEAN 0
#define DESERT 1
#define BRICK 2
#define SHEEP 3
#define WOOD 4
#define STONE 5
#define WHEAT 6

class Mode
{
public:
    Mode(const SegmentDisplay &disp, const LEDStatusDisplay &leds, const ButtonArray16 &btns, const SoftwareSerial &netPort) : disp(disp), leds(leds), btns(btns), netPort(netPort)
    {
        Serial.println(F("Construct"));
        init();
    }
    virtual void init() {}
    virtual void process(unsigned long currentMillis, uint16_t state){};

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
};

class ComponentTestMode : public Mode
{
public:
    using Mode::Mode;

    virtual void init();
    virtual void process(unsigned long currentMillis, uint16_t state);

private:
    unsigned long previousMillisLeds = 0;
};

class NetworkTestMode : public Mode
{
public:
    using Mode::Mode;

    virtual void init();
    virtual void process(unsigned long currentMillis, uint16_t state);

private:
    NodeId_t myId = EMPTY;
    NodeId_t neighborIds[6];
    char displayMessage[100] = {0};

    void processMessage(const Message &message);
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
    virtual void process(unsigned long currentMillis, uint16_t state);

private:
    static const __int24 PLAYER_COLORS[6];
    static const __int24 LAND_COLORS[8];

    NodeId_t myId = EMPTY;
    NodeId_t neighborIds[6];
    short roadOwners[NUM_ROADS];
    short settlementOwners[NUM_SETTLEMENTS];
    bool isCity[NUM_SETTLEMENTS] = {false};
    byte landType = -1;
    byte rollValue = 0;
    bool hasRobber = false;
    bool playerSelectMode = false;
    byte currentPlayer = 0;
    bool playStarted = false;
    byte setupStage = 0;
    char displayValue[10] = {0};

    static __int24 getPlayerColoer(byte playerNumber);
    static __int24 getLandColor(byte landNumber);
    
    void updateRoads(uint16_t state);
    void updateSettlements(uint16_t state);
    void updateRobber(uint16_t state);
    void updateCurrentPlayer(uint16_t state);
    void renderState();
    void setupGame();
    void setTileValue(byte val);
    void processMessage(const Message &message);
    void advanceSetupStage(byte stage);
};

#endif // _MODES_H_

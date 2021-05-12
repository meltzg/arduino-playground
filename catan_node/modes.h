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

#define MAX_NET_RETRIES 1000

// Mode Specific Defines
// Netowrk test
#define BTN_ID 9
#define BTN_NEIGHBORS 1
#define BTN_DISCOVER 10

// Catan
#define ALL_LAND true
#define LED_LAND 10
#define BTN_LAND 9
#define SEED_PIN A5

#define CATAN_SETUP_NEIGHBORS 1

#define PLAYER_SELECT_DELAY 500

#define NUM_ROADS 6
#define NUM_SETTLEMENTS 2
#define UNOWNED 0xFF

#define NUM_DICE 2
#define DIE_SIDES 6
#define MAX_ROLL NUM_DICE *DIE_SIDES
#define MIN_ROLL NUM_DICE

class Mode
{
public:
    Mode(const SegmentDisplay &disp, const LEDStatusDisplay &leds, const ButtonArray16 &btns, const SoftwareSerial &netPort) : disp(disp), leds(leds), btns(btns), netPort(netPort)
    {
        Serial.println(F("Construct"));
        init();
    }
    virtual void init() {}
    virtual void processState(unsigned long currentMillis, uint16_t state) = 0;
    virtual void processMessage(const Message &message) = 0;

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

    void init();
    void processState(unsigned long currentMillis, uint16_t state);
    void processMessage(const Message &message) {}

private:
    unsigned long previousMillisLeds = 0;
};

class NetworkTestMode : public Mode
{
public:
    using Mode::Mode;

    void init();
    void processState(unsigned long currentMillis, uint16_t state);
    void processMessage(const Message &message);

private:
    NodeId_t myId = EMPTY;
    NodeId_t neighborIds[6];
    char displayMessage[100] = {0};

    void handleNodeResponse(const Message &message);
    void handleIdRequest();
    void handleNeighborRequest(NodeId_t destination);
    void handleDiscoveryRequest() {}
};

class CatanLandType
{
public:
    enum Value : byte
    {
        OCEAN,
        DESERT,
        BRICK,
        SHEEP,
        WOOD,
        STONE,
        WHEAT,
        NONE,
    };

    CatanLandType() = default;
    constexpr CatanLandType(Value aType) : value(aType) {}

    operator Value() const { return value; }
    explicit operator bool() = delete;

    __int24 toGRB() const
    {
        switch (value)
        {
        case OCEAN:
            return 0x3813BE;
        case DESERT:
            return 0xB4D28C;
        case BRICK:
            return 0x41CB54;
        case SHEEP:
            return 0xFCB038;
        case WOOD:
            return 0xAC0313;
        case STONE:
            return 0xED3D97;
        case WHEAT:
            return YELLOW;
        default:
            return BLACK;
        }
    }

private:
    Value value;
};

enum CatanCommand : byte
{
    SET_ROAD,
    GET_STATE,
    STATE_RESPONSE
};

struct CatanState
{
    byte roadOwners[NUM_ROADS];
    byte settlementOwners[NUM_SETTLEMENTS];
    bool isCity[NUM_SETTLEMENTS] = {false};
    CatanLandType landType = CatanLandType::NONE;
    byte rollValue = 0;
    bool hasRobber = false;
};

struct CatanMessage
{
    CatanCommand command;
};

struct SetRoadRequest : public CatanMessage
{
    byte roadNumber;
    byte playerNumber;

    SetRoadRequest(byte roadNumber, byte playerNumber) : roadNumber(roadNumber), playerNumber(playerNumber) { command = SET_ROAD; }
};

class PlacementValidationInfo
{
public:
    enum Value : byte
    {
        ROAD,
        SETTLEMENT,
        NONE,
    };

    PlacementValidationInfo() = default;
    constexpr PlacementValidationInfo(Value aType) : value(aType) {}

    operator Value() const { return value; }
    explicit operator bool() = delete;

private:
    Value value;
    byte toValidate = 0xFF;
    byte playerNumber = UNOWNED;
    bool onLand = false;
};

struct GetStateRequest : public CatanMessage
{
    PlacementValidationInfo placementInfo = PlacementValidationInfo::NONE;

    GetStateRequest(PlacementValidationInfo placementInfo) : placementInfo(placementInfo) { command = GET_STATE; }
};

struct StateResponse : public CatanMessage
{
    CatanState state;
    PlacementValidationInfo placementInfo = PlacementValidationInfo::NONE;

    StateResponse(PlacementValidationInfo placementInfo, CatanState state) : placementInfo(placementInfo), state(state) { command = STATE_RESPONSE; }
};

class CatanMode : public Mode
{
public:
    using Mode::Mode;

    void init();
    void processState(unsigned long currentMillis, uint16_t state);
    void processMessage(const Message &message);

private:
    static const __int24 PLAYER_COLORS[6];
    static const __int24 LAND_COLORS[8];

    NodeId_t myId = EMPTY;
    NodeId_t neighborIds[6];
    CatanState catanState;
    bool playerSelectMode = false;
    byte currentPlayer = 0;
    bool playStarted = false;
    byte setupStage = 0;
    char displayValue[10] = {0};

    static __int24 getPlayerColoer(byte playerNumber);

    void updateRoads(uint16_t state);
    void updateSettlements(uint16_t state);
    void updateRobber(uint16_t state);
    void updateCurrentPlayer(uint16_t state);
    void renderState();
    void setupGame();
    void setTileValue(byte val);
    void advanceSetupStage(byte stage);

    void setRoadOwner(SetRoadRequest request, bool updateNeighbor = true);
    void sendStateRequest(NodeId_t node, PlacementValidationInfo placementInfo);
};

#endif // _MODES_H_

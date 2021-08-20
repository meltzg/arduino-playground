#ifndef _MODES_H_
#define _MODES_H_

#include <SoftwareSerial.h>
#include <CommonMessaging.h>
#include <Components.h>
#include <DataStructures.h>

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

// Mode definitions
#define NUM_MODES 3
#define MODE_COMPONENT_TEST 0
#define MODE_NETWORK_TEST 1
#define MODE_CATAN 2

// Mode Specific Defines
// Netowrk test
#define BTN_ID 9
#define BTN_NEIGHBORS 1
#define BTN_DISCOVER 3

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

// number of tiles based on base game + 6 player expansion
#define NUM_WOOD_TILES 6
#define NUM_SHEEP_TILES 6
#define NUM_WHEAT_TILES 6
#define NUM_BRICK_TILES 5
#define NUM_STONE_TILES 5
#define NUM_DESERT_TILES 2

struct ModeMessage
{
    byte modeId;
};

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

    NodeId_t myId = EMPTY;

    unsigned long previousMillis = 0;
    uint16_t previousState = 0;

    char displayMessage[50] = {0};
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

class DiscoveryMode : public Mode
{
public:
    using Mode::Mode;

    virtual void processState(unsigned long currentMillis, uint16_t state);
    virtual void processMessage(const Message &message);

protected:
    int btnDiscover = 0;

    bool pollDiscovery = false;
    bool postDiscovery = false;

    bool sendIdRequest();
    bool sendDiscoveryRequest();
    bool sendDiscoveryStatsRequest();
    bool sendNeighborRequest(NodeId_t destination, bool useCache = false);
    void handleDiscoveryStatsResponse(const Message &message);
    virtual void handleNodeResponse(const Message &message) = 0;
    virtual void doPostDiscovery() = 0;

private:
    unsigned long previousDiscoveryMillis = 0;
};

enum NetworkTestCommand : byte
{
    START_NODE
};

struct NetworkTestMessage : public ModeMessage
{
    NetworkTestCommand command;
};

struct WakeNodeMessage : public NetworkTestMessage
{
    WakeNodeMessage()
    {
        modeId = MODE_NETWORK_TEST;
        command = START_NODE;
    }
};

class NetworkTestMode : public DiscoveryMode
{
public:
    using DiscoveryMode::DiscoveryMode;

    void init();
    void processState(unsigned long currentMillis, uint16_t state);
    void processMessage(const Message &message);

protected:
    void doPostDiscovery();

private:
    NodeId_t neighborIds[6];
    Set<NodeId_t> discoveryVisited;
    LinkedList<NodeId_t> discoveryQueue;

    void handleNodeResponse(const Message &message);
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

    const char *toString()
    {
        switch (value)
        {
        case OCEAN:
            return "Ocean ";
        case DESERT:
            return "Desert ";
        case BRICK:
            return "Brick ";
        case SHEEP:
            return "Sheep ";
        case WOOD:
            return "Wood ";
        case STONE:
            return "Stone ";
        case WHEAT:
            return "Wheat ";
        default:
            return "";
        }
    }

    byte toWeight(bool includeDesert = false)
    {
        switch (value)
        {
        case OCEAN:
            return 0;
        case DESERT:
            return includeDesert ? NUM_DESERT_TILES : 0;
        case BRICK:
            return NUM_BRICK_TILES;
        case SHEEP:
            return NUM_SHEEP_TILES;
        case WOOD:
            return NUM_WOOD_TILES;
        case STONE:
            return NUM_STONE_TILES;
        case WHEAT:
            return NUM_WHEAT_TILES;
        default:
            return 0;
        }
    }

    static CatanLandType randomType(bool includeDesert = false);
    static int numDessertTiles(int numLandTiles);

private:
    Value value;
};

enum CatanCommand : byte
{
    SET_ROAD,
    SET_INITIAL_STATE,
    GET_STATE,
    STATE_RESPONSE,
    SET_PLAYER
};

struct BaseCatanState
{
    CatanLandType landType = CatanLandType::NONE;
    byte rollValue = 0;
    bool hasRobber = false;
};

struct CatanState : public BaseCatanState
{
    NodeId_t id = EMPTY;
    NodeId_t controllerId;
    byte roadOwners[NUM_ROADS];
    byte settlementOwners[NUM_SETTLEMENTS];
    bool isCity[NUM_SETTLEMENTS] = {false};
};

struct CatanMessage : public ModeMessage
{
    CatanCommand command;
};

struct SetRoadRequest : public CatanMessage
{
    byte roadNumber;
    byte playerNumber;

    SetRoadRequest(byte roadNumber, byte playerNumber) : roadNumber(roadNumber), playerNumber(playerNumber)
    {
        modeId = MODE_CATAN;
        command = SET_ROAD;
    }
};

struct SetInitialStateRequest : public CatanMessage
{
    BaseCatanState initialState;

    SetInitialStateRequest(BaseCatanState initialState) : initialState(initialState)
    {
        modeId = MODE_CATAN;
        command = SET_INITIAL_STATE;
    }
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

    byte toValidate = 0xFF;      // the road/settlement number being validated
    byte validateStep = 0;       // how many validation steps have been completed
    byte playerNumber = UNOWNED; // the player requesting ownership
    bool onLand = false;         // whether we've seen valid land for the placement

private:
    Value value; // type of validation to do
};

struct GetStateRequest : public CatanMessage
{
    PlacementValidationInfo placementInfo = PlacementValidationInfo::NONE;

    GetStateRequest(PlacementValidationInfo placementInfo) : placementInfo(placementInfo)
    {
        modeId = MODE_CATAN;
        command = GET_STATE;
    }
};

struct StateResponse : public CatanMessage
{
    CatanState state;
    PlacementValidationInfo placementInfo = PlacementValidationInfo::NONE;

    StateResponse(PlacementValidationInfo placementInfo, CatanState state) : placementInfo(placementInfo), state(state)
    {
        modeId = MODE_CATAN;
        command = STATE_RESPONSE;
    }
};

struct SetCurrentPlayerRequest: public CatanMessage
{
    byte playerNumber = 0;

    SetCurrentPlayerRequest(byte playerNumber) : playerNumber(playerNumber)
    {
        modeId = MODE_CATAN;
        command = SET_PLAYER;
    }
};

class CatanMode : public DiscoveryMode
{
public:
    using DiscoveryMode::DiscoveryMode;

    void init();
    void processState(unsigned long currentMillis, uint16_t state);
    void processMessage(const Message &message);

protected:
    void handleNodeResponse(const Message &message);
    void doPostDiscovery();

private:
    static const __int24 PLAYER_COLORS[6];
    static const __int24 LAND_COLORS[8];

    Map<NodeId_t, BaseCatanState> initialStates;
    LinkedList<NodeId_t> discoveryQueue;

    NodeId_t neighborIds[6];
    CatanState catanState;
    bool playerSelectMode = false;
    byte currentPlayer = 0;
    bool playStarted = false;

    static __int24 getPlayerColor(byte playerNumber);

    void updateRoads(uint16_t state);
    void updateSettlements(uint16_t state);
    void updateRobber(uint16_t state);
    void updateCurrentPlayer(uint16_t state);
    void renderState();
    void setTileValue(byte val);

    void setRoadOwner(SetRoadRequest request, bool updateNeighbor = true);
    void setInitialState(NodeId_t node, SetInitialStateRequest request);
    void sendSetInitialStateRequest(NodeId_t node, SetInitialStateRequest request);
    void sendStateRequest(NodeId_t node, PlacementValidationInfo placementInfo);
    void sendStateResponse(NodeId_t node, PlacementValidationInfo placementInfo);
    void sendSetCurrentPlayerRequest();
    void setCurrentPlayer(SetCurrentPlayerRequest request);

    void reconcileStateResponse(StateResponse response);
    void reconcileSettlementValidation(StateResponse response);
    void reconcileRoadValidation(StateResponse response);

    void setupBoard();
};

#endif // _MODES_H_

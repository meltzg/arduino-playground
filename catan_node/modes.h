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
#define MAX_NET_COMPLETE_RETRIES 5
#define MAX_NET_COMPLETE_RETRY_DELAY 500

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
#define RENDER_PORTS true
#define LED_LAND 10
#define BTN_LAND 9
#define SEED_PIN A5

#define CATAN_SETUP_NEIGHBORS 1

#define PLAYER_SELECT_DELAY 500
#define START_OVER_DELAY 5000

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

// Hardware config
#define SEGMENT_LATCH A0
#define SEGMENT_CLOCK A1
#define SEGMENT_DATA A2
#define SEGMENT_LEFT A3
#define SEGMENT_RIGHT A4

#define LED_ARRAY 7
#define NUM_LEDS 11

#define NUM_BUTTONS 10
#define BTN_LOAD 10
#define BTN_CLK_ENABLE 11
#define BTN_DATA 12
#define BTN_CLOCK 13

#define BTN_CENTER 8

const byte EDGE_LED_POS[6] = {0, 3, 6, 7, 8, 9};
const byte EDGE_BTN_POS[6] = {0, 2, 4, 5, 6, 7};
const byte CORNER_LED_POS[2][2] = {{1, 2}, {4, 5}};
const byte CORNER_BTN_POS[2] = {1, 3};

// Component setup
extern SegmentDisplay disp;

extern LEDStatusDisplay leds;

extern ButtonArray16 btns;

extern SoftwareSerial netPort;

// Mode types

struct ModeMessage
{
    byte modeId;
};

// Mode State

extern unsigned long previousMillis;
extern unsigned long previousMillisLeds;
extern uint16_t previousState;

extern char displayMessage[50];

// Networking types

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

// Mode State for networking

extern int btnDiscover;

extern bool pollDiscovery;
extern bool postDiscovery;

extern Set<NodeId_t> discoveryVisited;
extern LinkedList<NodeId_t> discoveryQueue;

// Methods for networking

bool sendIdRequest();
bool sendDiscoveryRequest();
bool sendDiscoveryStatsRequest();
bool sendNeighborRequest(NodeId_t destination, bool useCache = false);
void handleIdResponse(const Message &message);
void handleDiscoveryStatsResponse(const Message &message);
void handleNodeResponseNetworkTest(const Message &message);

// Catan types

class CatanLandType
{
public:
    enum Value : byte
    {
        NONE,
        OCEAN,
        DESERT,
        BRICK,
        SHEEP,
        WOOD,
        STONE,
        WHEAT,
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
        byte weight = 0;
        switch (value)
        {
        case DESERT:
            weight = includeDesert ? NUM_DESERT_TILES : 0;
            break;
        case BRICK:
            weight = NUM_BRICK_TILES;
            break;
        case SHEEP:
            weight = NUM_SHEEP_TILES;
            break;
        case WOOD:
            weight = NUM_WOOD_TILES;
            break;
        case STONE:
            weight = NUM_STONE_TILES;
            break;
        case WHEAT:
            weight = NUM_WHEAT_TILES;
            break;
        default:
            return 0;
        }

        return weight - landWeightOffsets[value];
    }
    byte toHarborWeight()
    {
        byte weight = 0;
        switch (value)
        {
        case DESERT:
            weight = 5;
            break;
        case BRICK:
            weight = 1;
            break;
        case SHEEP:
            weight = 2;
            break;
        case WOOD:
            weight = 1;
            break;
        case STONE:
            weight = 1;
            break;
        case WHEAT:
            weight = 1;
            break;
        default:
            return 0;
        }

        return weight - harborWeightOffsets[value];
    }

    static CatanLandType randomType(bool includeDesert = false);
    static CatanLandType randomHarbor();
    static int numDesertTiles(int numLandTiles);
    static int numHarborTiles(int numLandTiles, int numOceanTiles);

private:
    Value value;
    static byte landWeightOffsets[WHEAT + 1];
    static byte harborWeightOffsets[WHEAT + 1];

    static void resetLandWeights();
    static void resetHarborWeights();
};

enum CatanCommand : byte
{
    SET_ROAD,
    SET_INITIAL_STATE,
    GET_STATE,
    STATE_RESPONSE,
    SET_PLAYER,
    CLEAR_ROBBER
};

struct BaseCatanState
{
    NodeId_t id = EMPTY;
    CatanLandType landType = CatanLandType::NONE;
    CatanLandType portType = CatanLandType::NONE;
    NodeId_t portNeighbor = EMPTY;
    byte rollValue = 0;
    bool hasRobber = false;
};

struct CatanState : public BaseCatanState
{
    NodeId_t controllerId;
    byte roadOwners[NUM_ROADS];
    byte settlementOwners[NUM_SETTLEMENTS];
    bool isCity[NUM_SETTLEMENTS] = {false};
};

struct CatanPlayState : public CatanState
{
    bool playStarted;
    NodeId_t neighborIds[6] = {EMPTY};
    byte currentPlayer = 0;
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

struct SetCurrentPlayerRequest : public CatanMessage
{
    byte playerNumber = 0;

    SetCurrentPlayerRequest(byte playerNumber) : playerNumber(playerNumber)
    {
        modeId = MODE_CATAN;
        command = SET_PLAYER;
    }
};

struct ClearRobberRequest : public CatanMessage
{
    NodeId_t robberId;

    ClearRobberRequest(NodeId_t robberId) : robberId(robberId)
    {
        modeId = MODE_CATAN;
        command = CLEAR_ROBBER;
    }
};

// Mode State for catan

extern Graph<NodeId_t> topology;

extern CatanPlayState catanState;
extern bool playerSelectMode; 
extern byte newPlayer;

// Methods for catan

void handleNodeResponseCatan(const Message &message);

__int24 getPlayerColor(byte playerNumber);

void updateRoads(uint16_t state);
void updateSettlements(uint16_t state);
void updateRobber(uint16_t state);
void updateCurrentPlayer(uint16_t state);
void renderState();
void setTileValue(byte val);

byte randomRollValue();

void setRoadOwner(SetRoadRequest request, bool updateNeighbor = true);
void setInitialState(NodeId_t node, SetInitialStateRequest request);
void sendSetInitialStateRequest(NodeId_t node, SetInitialStateRequest request);
void sendStateRequest(NodeId_t node, PlacementValidationInfo placementInfo);
void sendStateResponse(NodeId_t node, PlacementValidationInfo placementInfo);
void sendSetCurrentPlayerRequest();
void sendClearRobberRequest();
void setCurrentPlayer(SetCurrentPlayerRequest request);

void reconcileStateResponse(StateResponse response);
void reconcileSettlementValidation(StateResponse response);
void reconcileRoadValidation(StateResponse response);

void setupBoard();
void saveState();
void loadState();

#endif // _MODES_H_

#include "modes.h"
#include "PathFinder.h"

SegmentDisplay disp = SegmentDisplay(
    SEGMENT_LATCH,
    SEGMENT_CLOCK,
    SEGMENT_DATA,
    SEGMENT_LEFT,
    SEGMENT_RIGHT,
    500,
    false);

LEDStatusDisplay leds = LEDStatusDisplay(LED_ARRAY, NUM_LEDS);

ButtonArray16 btns = ButtonArray16(
    BTN_LOAD,
    BTN_CLK_ENABLE,
    BTN_DATA,
    BTN_CLOCK);

SoftwareSerial netPort = SoftwareSerial(8, 9);

unsigned long previousMillis = 0;
unsigned long previousMillisLeds = 0;
uint16_t previousState = 0;

char displayMessage[50] = {0};

int btnDiscover = 0;

bool pollDiscovery = false;
bool postDiscovery = false;
CatanResetType resetType = CatanResetType::NONE;

Set<NodeId_t> discoveryVisited;
LinkedList<NodeId_t> discoveryQueue;

Graph<NodeId_t> topology = Graph<NodeId_t>(true, 0, EEPROM.length());

CatanPlayState catanState;
bool playerSelectMode = false;
byte newPlayer = 0;

static byte CatanLandType::landWeightOffsets[WHEAT + 1] = {0};
static byte CatanLandType::harborWeightOffsets[WHEAT + 1] = {0};

bool sendIdRequest()
{
    Message idRequest(
        EMPTY,
        EMPTY,
        0,
        0,
        ROUTER_GET_ID,
        NULL);
    catanState.id = 0;

    if (!writeMessage(&netPort, idRequest, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        disp.setChars("ID req Failure ");
        return false;
    }

    return true;
}

bool sendDiscoveryRequest()
{
    if (catanState.id == EMPTY)
    {
        return false;
    }
    topology.purge();
    Message discoveryRequest(
        catanState.id,
        catanState.id,
        0,
        ROUTER_ENABLE_DISCOVERY_UPDATES,
        ROUTER_START_DISCOVERY,
        NULL);

    if (!writeMessage(&netPort, discoveryRequest, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        disp.setChars("D req Failure ");
        return false;
    }
    return true;
}

bool sendDiscoveryStatsRequest()
{
    Message discoveryStatsRequest(
        catanState.id,
        catanState.id,
        0,
        0,
        ROUTER_GET_DISCOVERY_STATUS,
        NULL);

    if (!writeMessage(&netPort, discoveryStatsRequest, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        disp.setChars("D stat Failure ");
        return false;
    }
    return true;
}

bool sendNeighborRequest(NodeId_t destination, bool useCache)
{
    Serial.print(F("Req neighbors "));
    Serial.println(destination, HEX);
    Serial.print(F("Use Cache "));
    Serial.println(useCache);
    Message neighborRequest(
        catanState.id,
        destination,
        0,
        useCache ? ROUTER_USE_CACHE : 0,
        ROUTER_GET_NEIGHBORS,
        NULL);

    if (!writeMessage(&netPort, neighborRequest, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        disp.setChars("N req Failure ");
        return false;
    }
    return true;
}

void handleIdResponse(const Message &message)
{
    catanState.id = *((NodeId_t *)message.getBody());
    sprintf(displayMessage, "My ID: %02X ", catanState.id);
    disp.setChars(displayMessage);
    Serial.println(displayMessage);
}

void handleDiscoveryStatsResponse(const Message &message)
{
    Serial.println(F("disc stat resp"));
    DiscoveryStats *stats = (DiscoveryStats *)message.getBody();
    sprintf(displayMessage, "D: %d, N: %d, E: %d    ", stats->discoveryDone, stats->numNodes, stats->numEdges);
    disp.setChars(displayMessage);
    Serial.println(displayMessage);
    if (stats->discoveryDone)
    {
        pollDiscovery = false;
        postDiscovery = true;
        resetType = CatanResetType::NONE;
        sendNeighborRequest(catanState.id, true);
    }
}

void handleNodeResponseNetworkTest(const Message &message)
{
    NodeId_t *nodes = ((NodeId_t *)(message.getBody()));
    if (!postDiscovery)
    {
        __int24 ledColors[leds.getNumLeds()] = {BLACK};
        for (int i = 0; i < 6; i++)
        {
            NodeId_t id = nodes[i + 1];
            Serial.println(id, HEX);
            if (id == EMPTY)
            {
                ledColors[EDGE_LED_POS[i]] = RED;
            }
            else
            {
                ledColors[EDGE_LED_POS[i]] = GREEN;
            }
            if (message.getSource() == catanState.id)
            {
                catanState.neighborIds[i] = id;
            }
        }
        leds.setState(ledColors);
    }
    else
    {
        int numNodes = message.getPayloadSize() / sizeof(NodeId_t);
        Serial.print(F("# nodes "));
        Serial.println(numNodes);
        NodeId_t id = nodes[0];
        discoveryVisited.pushBack(id);
        for (int i = 1; i < numNodes; i++)
        {
            id = nodes[i];
            if (id != EMPTY && !discoveryVisited.contains(id))
            {
                Serial.println(i);
                Serial.println(id, HEX);
                discoveryQueue.pushBack(id);
                discoveryVisited.pushBack(id);
                Serial.println(discoveryQueue.peekFront(), HEX);
            }
        }

        if (discoveryQueue.isEmpty())
        {
            Serial.println(F("crawl done"));
            discoveryQueue.purge();
            discoveryVisited.purge();
            postDiscovery = false;
            sendNeighborRequest(catanState.id, true);
        }
        else
        {
            NodeId_t nextNode = discoveryQueue.popFront();

            WakeNodeMessage command;
            Message msg(
                catanState.id,
                nextNode,
                sizeof(WakeNodeMessage),
                0,
                0,
                (byte *)&command);

            Serial.print(F("Wake "));
            Serial.println(nextNode, HEX);

            if (msg.getDest() != EMPTY)
            {
                if (!writeMessage(&netPort, msg, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
                {
                    Serial.println(F("Fail"));
                }
            }
            sendNeighborRequest(nextNode, true);
        }
    }
}

CatanLandType CatanLandType::randomType(bool includeDesert)
{
    int totalWeight = 0;
    while (totalWeight == 0)
    {
        for (int i = CatanLandType::NONE; i <= CatanLandType::WHEAT; i++)
        {
            totalWeight += static_cast<CatanLandType>(i).toWeight(includeDesert);
        }
        if (totalWeight == 0)
        {
            resetLandWeights();
        }
    }

    Serial.print(F("Total land weight "));
    Serial.println(totalWeight);

    int rnd = random(totalWeight);
    for (int i = CatanLandType::NONE; i <= CatanLandType::WHEAT; i++)
    {
        if (rnd < static_cast<CatanLandType>(i).toWeight(includeDesert))
        {
            CatanLandType land = static_cast<CatanLandType>(i);
            landWeightOffsets[land]++;
            return land;
        }
        rnd -= static_cast<CatanLandType>(i).toWeight(includeDesert);
    }

    return CatanLandType::NONE;
}

CatanLandType CatanLandType::randomHarbor()
{
    int totalWeight = 0;
    while (totalWeight == 0)
    {
        for (int i = CatanLandType::NONE; i <= CatanLandType::WHEAT; i++)
        {
            totalWeight += static_cast<CatanLandType>(i).toHarborWeight();
        }
        if (totalWeight == 0)
        {
            resetHarborWeights();
        }
    }

    Serial.print(F("Total harbor weight "));
    Serial.println(totalWeight);

    int rnd = random(totalWeight);
    for (int i = CatanLandType::NONE; i <= CatanLandType::WHEAT; i++)
    {
        if (rnd < static_cast<CatanLandType>(i).toHarborWeight())
        {
            CatanLandType harbor = static_cast<CatanLandType>(i);
            harborWeightOffsets[harbor]++;
            return harbor;
        }
        rnd -= static_cast<CatanLandType>(i).toHarborWeight();
    }

    return CatanLandType::NONE;
}

int CatanLandType::numDesertTiles(int numLandTiles)
{
    return numLandTiles / 30 + 1;
}

int CatanLandType::numHarborTiles(int numLandTiles, int numOceanTiles)
{
    return min(numOceanTiles, (2 * (numLandTiles / 30) + 9));
}

void CatanLandType::resetLandWeights()
{
    Serial.println(F("Reset Lands"));
    for (int i = CatanLandType::NONE; i <= CatanLandType::WHEAT; i++)
    {
        landWeightOffsets[i] = 0;
    }
}

void CatanLandType::resetHarborWeights()
{
    Serial.println(F("Reset Harbors"));
    for (int i = CatanLandType::NONE; i <= CatanLandType::WHEAT; i++)
    {
        harborWeightOffsets[i] = 0;
    }
}

void handleNodeResponseCatan(const Message &message)
{
    NodeId_t *nodes = ((NodeId_t *)(message.getBody()));

    if (!catanState.playStarted)
    {
        int numNodes = message.getPayloadSize() / sizeof(NodeId_t);
        Serial.print(F("# nodes "));
        Serial.println(numNodes);
        NodeId_t sourceId = nodes[0];

        discoveryVisited.pushBack(sourceId);

        for (int i = 1; i < numNodes; i++)
        {
            NodeId_t sinkId = nodes[i];
            if (sinkId != EMPTY && !discoveryVisited.contains(sinkId))
            {
                topology.addEdge(nodes[0], sinkId);
                Serial.println(i);
                Serial.println(sinkId, HEX);
                if (!discoveryQueue.contains(sinkId))
                {
                    discoveryQueue.pushBack(sinkId);
                }
                Serial.println(discoveryQueue.peekFront(), HEX);
            }
        }

        if (discoveryQueue.isEmpty())
        {
            Serial.println(F("Crawl Complete"));
            discoveryQueue.purge();
            discoveryVisited.purge();
            postDiscovery = false;
            setupBoard();
        }
        else
        {
            NodeId_t nextNode = discoveryQueue.popFront();
            sendNeighborRequest(nextNode, true);
        }
    }
    else if (message.getSource() == catanState.id)
    {
        for (int i = 0; i < 6; i++)
        {
            NodeId_t id = ((NodeId_t *)(message.getBody()))[i + 1];
            catanState.neighborIds[i] = id;
            Serial.println(id, HEX);
        }
    }
}

void updateRoads(uint16_t state)
{
    for (int i = 0; i < NUM_ROADS; i++)
    {
        byte btnPos = EDGE_BTN_POS[i];

        if (((previousState >> btnPos) & 1) && ((state >> btnPos) & 1) == 0)
        {
            Serial.print(F("currentOwner: "));
            Serial.println(catanState.roadOwners[i]);
            if (catanState.roadOwners[i] == UNOWNED)
            {
                PlacementValidationInfo validation = PlacementValidationInfo::ROAD;
                validation.toValidate = i;
                validation.playerNumber = catanState.currentPlayer;
                reconcileRoadValidation(StateResponse(validation, catanState));
            }
            else if (catanState.roadOwners[i] == catanState.currentPlayer)
            {
                setRoadOwner(SetRoadRequest(i, UNOWNED));
            }
        }
    }
}

void updateSettlements(uint16_t state)
{
    for (int i = 0; i < NUM_SETTLEMENTS; i++)
    {
        byte btnPos = CORNER_BTN_POS[i];

        if (((previousState >> btnPos) & 1) && ((state >> btnPos) & 1) == 0)
        {
            if (catanState.settlementOwners[i] == UNOWNED)
            {
                PlacementValidationInfo validation = PlacementValidationInfo::SETTLEMENT;
                validation.toValidate = i;
                validation.playerNumber = catanState.currentPlayer;
                reconcileSettlementValidation(StateResponse(validation, catanState));
            }
            else if (catanState.settlementOwners[i] == catanState.currentPlayer)
            {
                if (!catanState.isCity[i])
                {
                    catanState.isCity[i] = true;
                }
                else
                {
                    catanState.isCity[i] = false;
                    catanState.settlementOwners[i] = UNOWNED;
                }
            }
        }
    }
}

void updateRobber(uint16_t state)
{
    if (catanState.landType == CatanLandType::OCEAN)
    {
        catanState.hasRobber = false;
        return;
    }

    if (((previousState >> BTN_LAND) & 1) && ((state >> BTN_LAND) & 1) == 0)
    {
        if (!catanState.hasRobber)
        {
            catanState.hasRobber = true;
            setTileValue(0xFF);
            sendClearRobberRequest();
        }
    }
}

void updateCurrentPlayer(uint16_t state)
{
    for (int i = 0; i < NUM_ROADS; i++)
    {
        byte btnPos = EDGE_BTN_POS[i];

        if (((previousState >> btnPos) & 1) && ((state >> btnPos) & 1) == 0)
        {
            newPlayer = i;
            break;
        }
    }
}

void renderState()
{
    __int24 ledColors[leds.getNumLeds()] = {BLACK};

    if (playerSelectMode)
    {
        for (int i = 0; i < NUM_ROADS; i++)
        {
            byte ledPos = EDGE_LED_POS[i];
            ledColors[ledPos] = getPlayerColor(i);
        }
        for (int i = 0; i < NUM_SETTLEMENTS; i++)
        {
            byte *ledPos = CORNER_LED_POS[i];
            ledColors[ledPos[0]] = BLACK;
            ledColors[ledPos[1]] = BLACK;
        }
        ledColors[LED_LAND] = getPlayerColor(newPlayer);
    }
    else
    {
        for (int i = 0; i < NUM_ROADS; i++)
        {
            byte ledPos = EDGE_LED_POS[i];

            if (catanState.roadOwners[i] == UNOWNED || catanState.landType == CatanLandType::OCEAN)
            {
                ledColors[ledPos] = BLACK;
            }
            else
            {
                ledColors[ledPos] = getPlayerColor(catanState.roadOwners[i]);
            }
        }
        if (RENDER_PORTS && catanState.landType == CatanLandType::OCEAN && catanState.portType != CatanLandType::NONE)
        {

            for (int i = 0; i < 6; i++)
            {
                if (catanState.neighborIds[i] == catanState.portNeighbor)
                {
                    ledColors[EDGE_LED_POS[i]] = catanState.portType.toGRB();
                }
            }
        }
        for (int i = 0; i < NUM_SETTLEMENTS; i++)
        {
            byte *ledPos = CORNER_LED_POS[i];

            if (catanState.settlementOwners[i] == UNOWNED)
            {
                ledColors[ledPos[0]] = BLACK;
                ledColors[ledPos[1]] = BLACK;
            }
            else
            {
                ledColors[ledPos[0]] = getPlayerColor(catanState.settlementOwners[i]);
                if (catanState.isCity[i])
                {
                    ledColors[ledPos[1]] = getPlayerColor(catanState.settlementOwners[i]);
                }
            }
        }
        ledColors[LED_LAND] = catanState.landType.toGRB();
    }

    leds.setState(ledColors);
}

void setTileValue(byte val)
{
    if (catanState.portType != CatanLandType::NONE)
    {
        if (catanState.portType == CatanLandType::DESERT)
        {
            sprintf(displayMessage, "3/1  ");
        }
        else
        {
            sprintf(displayMessage, "2/1 %s ", catanState.portType.toString());
        }
    }
    else if (val < MIN_ROLL)
    {
        sprintf(displayMessage, "  ");
    }
    else if (val > MAX_ROLL)
    {
        sprintf(displayMessage, "Robber ");
    }
    else
    {
        sprintf(displayMessage, "%02d", val);
    }
    disp.setChars(displayMessage);
}

byte randomRollValue()
{
    byte robberValue = 0;
    if (NUM_DICE % 2 == 0 ^ DIE_SIDES % 2 != 0)
    {
        robberValue = (NUM_DICE * (DIE_SIDES + 1)) / 2;
    }
    else
    {
        robberValue = (NUM_DICE * (DIE_SIDES + 1) - 1) / 2;
    }
    Serial.print(F("Robber value: "));
    Serial.println(robberValue, DEC);
    byte rollValue = robberValue;
    while (rollValue == robberValue)
    {
        rollValue = random(NUM_DICE, NUM_DICE * DIE_SIDES);
    }
    return rollValue;
}

__int24 getPlayerColor(byte playerNumber)
{
    switch (playerNumber)
    {
    case 0:
        return RED;
    case 1:
        return ORANGE;
    case 2:
        return GREEN;
    case 3:
        return BLUE;
    case 4:
        return PURPLE;
    case 5:
        return WHITE;
    default:
        return BLACK;
    }
}

void setRoadOwner(SetRoadRequest request, bool updateNeighbor = true)
{
    catanState.roadOwners[request.roadNumber] = request.playerNumber;
    Serial.print(F("updateNeighbor: "));
    Serial.println(updateNeighbor);
    Serial.print(F("roadNumber: "));
    Serial.println(request.roadNumber);
    Serial.print(F("playerNumber: "));
    Serial.println(request.playerNumber);
    if (updateNeighbor)
    {
        SetRoadRequest command((request.roadNumber + NUM_ROADS / 2) % NUM_ROADS, request.playerNumber);
        Message msg(
            catanState.id,
            catanState.neighborIds[request.roadNumber],
            sizeof(SetRoadRequest),
            0,
            0,
            (byte *)&command);

        Serial.print(F("updateNeighbor: "));
        Serial.println(updateNeighbor);

        if (msg.getDest() != EMPTY)
        {
            if (!writeMessage(&netPort, msg, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
            {
                Serial.println(F("Fail"));
            }
        }
    }
}

void setInitialState(NodeId_t node, SetInitialStateRequest request)
{
    catanState.id = request.initialState.id;
    catanState.landType = request.initialState.landType;
    catanState.rollValue = request.initialState.rollValue;
    catanState.hasRobber = request.initialState.hasRobber;
    catanState.portType = request.initialState.portType;
    catanState.portNeighbor = request.initialState.portNeighbor;

    if (catanState.hasRobber)
    {
        setTileValue(0xFF);
    }
    else
    {
        setTileValue(catanState.rollValue);
    }

    for (int i = 0; i < NUM_ROADS; i++)
    {
        catanState.roadOwners[i] = UNOWNED;
    }
    for (int i = 0; i < NUM_SETTLEMENTS; i++)
    {
        catanState.settlementOwners[i] = UNOWNED;
    }

    catanState.playStarted = true;

    Serial.print(F("ID: "));
    Serial.println(catanState.id, HEX);

    setCurrentPlayer(SetCurrentPlayerRequest(0));

    sendNeighborRequest(catanState.id, true);
}

void setState(NodeId_t source, bool hardwareProxy, SetStateRequest request)
{
    bool roadsToSet[NUM_ROADS] = {false};
    for (int i = 0; i < NUM_ROADS; i++)
    {
        if (!request.validRoads[i])
        {
            request.state.roadOwners[i] = catanState.roadOwners[i];
        }
        if (catanState.roadOwners[i] != request.state.roadOwners[i])
        {
            roadsToSet[i] = true;
        }
    } 
    memcpy(&catanState, &request.state, sizeof(CatanPlayState));

    setTileValue(catanState.hasRobber ? 0xFF : catanState.rollValue);

    for (int i = 0; i < NUM_ROADS; i++)
    {
        if (!roadsToSet[i])
        {
            continue;
        }
        SetRoadRequest command((i + NUM_ROADS / 2) % NUM_ROADS, catanState.roadOwners[i]);
        Message msg(
            catanState.id,
            catanState.neighborIds[i],
            sizeof(SetRoadRequest),
            0,
            0,
            (byte *)&command);

        if (msg.getDest() != EMPTY)
        {
            if (!writeMessage(&netPort, msg, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
            {
                Serial.println(F("Fail"));
            }
        }
    }

    Serial.print(F("send ack? "));
    Serial.println(request.sendAck);
    if (request.sendAck)
    {
        sendPlayStateResponse(source, hardwareProxy);
    }
    setCurrentPlayer(SetCurrentPlayerRequest(catanState.currentPlayer));
}

void sendPlayStateResponse(NodeId_t source, bool hardwareProxy)
{
    PlayStateResponse ack(catanState);
    Message msg(
        catanState.id,
        source,
        sizeof(PlayStateResponse),
        hardwareProxy ? ROUTER_HARDWARE_PROXY_RESPONSE : 0,
        0,
        (byte *)&ack);

    if (!writeMessage(&netPort, msg, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        Serial.println(F("Fail"));
    }
}

void sendSetInitialStateRequest(NodeId_t node, SetInitialStateRequest request)
{
    Message msg(
        catanState.id,
        node,
        sizeof(SetInitialStateRequest),
        0,
        0,
        (byte *)&request);

    if (!writeMessage(&netPort, msg, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        Serial.println(F("Fail"));
    }
}

void sendStateRequest(NodeId_t node, PlacementValidationInfo placementInfo)
{
    GetStateRequest request(placementInfo);
    Message msg(
        catanState.id,
        node,
        sizeof(GetStateRequest),
        0,
        0,
        (byte *)&request);

    if (!writeMessage(&netPort, msg, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        Serial.println(F("Fail"));
    }
}

void sendStateResponse(NodeId_t node, PlacementValidationInfo placementInfo)
{
    StateResponse response(placementInfo, catanState);
    Message msg(
        catanState.id,
        node,
        sizeof(StateResponse),
        0,
        0,
        (byte *)&response);

    if (!writeMessage(&netPort, msg, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        Serial.println(F("Fail"));
    }
}

void sendSetCurrentPlayerRequest()
{
    SetCurrentPlayerRequest request(catanState.currentPlayer);
    Message msg(
        catanState.id,
        catanState.id,
        sizeof(SetCurrentPlayerRequest),
        ROUTER_BROADCAST,
        0,
        (byte *)&request);

    if (!writeMessage(&netPort, msg, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        Serial.println(F("Fail"));
    }
}

void sendClearRobberRequest()
{
    ClearRobberRequest request(catanState.id);
    Message msg(
        catanState.id,
        catanState.id,
        sizeof(ClearRobberRequest),
        ROUTER_BROADCAST,
        0,
        (byte *)&request);

    if (!writeMessage(&netPort, msg, MAX_NET_RETRIES, MAX_NET_COMPLETE_RETRIES, MAX_NET_COMPLETE_RETRY_DELAY))
    {
        Serial.println(F("Fail"));
    }
}

void setCurrentPlayer(SetCurrentPlayerRequest request)
{
    catanState.currentPlayer = request.playerNumber;
    newPlayer = catanState.currentPlayer;
    __int24 ledColors[leds.getNumLeds()] = {BLACK};
    ledColors[LED_LAND] = getPlayerColor(catanState.currentPlayer);
    leds.setState(ledColors);
    delay(500);
}

void reconcileStateResponse(StateResponse response)
{
    switch (response.placementInfo)
    {
    case PlacementValidationInfo::ROAD:
        reconcileRoadValidation(response);
        break;
    case PlacementValidationInfo::SETTLEMENT:
        reconcileSettlementValidation(response);
        break;
    default:
        Serial.println(F("No validation required"));
        break;
    }
}

void reconcileSettlementValidation(StateResponse response)
{
    if (response.placementInfo.toValidate >= NUM_SETTLEMENTS)
    {
        Serial.println(F("Invalid settlement index"));
        return;
    }

    Serial.print(F("step "));
    Serial.println(response.placementInfo.validateStep);

    uint8_t otherCity = (response.placementInfo.toValidate + 1) % NUM_SETTLEMENTS;
    bool isValid = true;
    NodeId_t nextTile = EMPTY;

    uint8_t otherCityTileIdx1 = response.placementInfo.toValidate * -3 + 5;
    uint8_t otherCityTileIdx2 = response.placementInfo.toValidate * 3;
    uint8_t landTileCheckIdx1 = response.placementInfo.toValidate;
    uint8_t landTileCheckIdx2 = response.placementInfo.toValidate + 1 % NUM_ROADS;

    Serial.print(F("to validate: "));
    Serial.println(response.placementInfo.toValidate);
    Serial.print(F("other tile idx 1: "));
    Serial.println(otherCityTileIdx1);
    Serial.print(F("other tile idx 2: "));
    Serial.println(otherCityTileIdx2);
    Serial.print(F("land check 1: "));
    Serial.println(landTileCheckIdx1);
    Serial.print(F("land check 2: "));
    Serial.println(landTileCheckIdx2);

    switch (response.placementInfo.validateStep)
    {
    case 0:
        // Check local city
        if (catanState.settlementOwners[otherCity] != UNOWNED)
        {
            isValid = false;
        }
        else
        {
            response.placementInfo.onLand = response.placementInfo.onLand || catanState.landType != CatanLandType::OCEAN;
            if (catanState.neighborIds[otherCityTileIdx1] != EMPTY)
            {
                response.placementInfo.validateStep = 1;
                nextTile = catanState.neighborIds[otherCityTileIdx1];
            }
            else if (catanState.neighborIds[otherCityTileIdx2] != EMPTY)
            {
                response.placementInfo.validateStep = 2;
                nextTile = catanState.neighborIds[otherCityTileIdx2];
            }
            else if (!response.placementInfo.onLand)
            {
                if (catanState.neighborIds[landTileCheckIdx1] != EMPTY)
                {
                    response.placementInfo.validateStep = 3;
                    nextTile = catanState.neighborIds[landTileCheckIdx1];
                }
                else if (catanState.neighborIds[landTileCheckIdx2] != EMPTY)
                {
                    response.placementInfo.validateStep = 4;
                    nextTile = catanState.neighborIds[landTileCheckIdx2];
                }
                else
                {
                    isValid = false;
                }
            }
        }
        break;
    case 1:
        // Check other city 1
        if (response.state.settlementOwners[otherCity] != UNOWNED)
        {
            isValid = false;
        }
        else
        {
            if (catanState.neighborIds[otherCityTileIdx2] != EMPTY)
            {
                response.placementInfo.validateStep = 2;
                nextTile = catanState.neighborIds[otherCityTileIdx2];
            }
            else if (!response.placementInfo.onLand)
            {
                if (catanState.neighborIds[landTileCheckIdx1] != EMPTY)
                {
                    response.placementInfo.validateStep = 3;
                    nextTile = catanState.neighborIds[landTileCheckIdx1];
                }
                else if (catanState.neighborIds[landTileCheckIdx2] != EMPTY)
                {
                    response.placementInfo.validateStep = 4;
                    nextTile = catanState.neighborIds[landTileCheckIdx2];
                }
                else
                {
                    isValid = false;
                }
            }
        }
        break;
    case 2:
        // Check other city 2
        if (response.state.settlementOwners[otherCity] != UNOWNED)
        {
            isValid = false;
        }
        else
        {
            if (!response.placementInfo.onLand)
            {
                if (catanState.neighborIds[landTileCheckIdx1] != EMPTY)
                {
                    response.placementInfo.validateStep = 3;
                    nextTile = catanState.neighborIds[landTileCheckIdx1];
                }
                else if (catanState.neighborIds[landTileCheckIdx2] != EMPTY)
                {
                    response.placementInfo.validateStep = 4;
                    nextTile = catanState.neighborIds[landTileCheckIdx2];
                }
                else
                {
                    isValid = false;
                }
            }
        }
        break;
    case 3:
        // Check land 1
        response.placementInfo.onLand = response.placementInfo.onLand || response.state.landType != CatanLandType::OCEAN;
        if (!response.placementInfo.onLand)
        {
            if (catanState.neighborIds[landTileCheckIdx2] != EMPTY)
            {
                response.placementInfo.validateStep = 4;
                nextTile = catanState.neighborIds[landTileCheckIdx2];
            }
            else
            {
                isValid = false;
            }
        }
        break;
    case 4:
        // check land 2
        response.placementInfo.onLand = response.placementInfo.onLand || response.state.landType != CatanLandType::OCEAN;
        if (!response.placementInfo.onLand)
        {
            isValid = false;
        }
        break;
    }

    if (!isValid)
    {
        Serial.println(F("Invalid settlment"));
    }
    else if (nextTile != EMPTY)
    {
        sendStateRequest(nextTile, response.placementInfo);
    }
    else
    {
        Serial.println(F("Settlement place approved"));
        catanState.settlementOwners[response.placementInfo.toValidate] = response.placementInfo.playerNumber;
    }
}

void setupBoard()
{
    Serial.println(F("Setup board"));

    int numLand = 0;
    Set<NodeId_t> adj;
    for (GraphIterator<NodeId_t> iter(topology, catanState.id); iter.hasNext();)
    {
        NodeId_t curr = iter.next();
        topology.getAdjacent(curr, adj);
        if (ALL_LAND || adj.count == 0 || adj.count == 6)
        {
            numLand++;
        }
    }

    Serial.print(F("# land "));
    Serial.println(numLand);
    Serial.print(F("# sea "));
    Serial.println(topology.numNodes() - numLand);

    // get indicies for desert tiles
    const int numDesert = CatanLandType::numDesertTiles(numLand);
    int desertTiles[numDesert];
    for (int i = 0; i < numDesert; i++)
    {
        desertTiles[i] = -1;
    }
    for (int i = 0; i < numDesert; i++)
    {
        while (desertTiles[i] < 0)
        {
            int val = random(numLand);
            bool valid = true;
            for (int j = 0; j < numDesert; j++)
            {
                if (desertTiles[j] == val)
                {
                    valid = false;
                    break;
                }
            }
            if (valid)
            {
                desertTiles[i] = val;
            }
        }
    }
    Serial.print(F("# desert "));
    Serial.println(numDesert);

    // get indicies for harbor tiles
    const int numHarbor = CatanLandType::numHarborTiles(numLand, topology.numNodes() - numLand);
    int harborTiles[numHarbor];
    for (int i = 0; i < numHarbor; i++)
    {
        harborTiles[i] = -1;
    }
    for (int i = 0; i < numHarbor; i++)
    {
        while (harborTiles[i] < 0)
        {
            int val = random(topology.numNodes() - numLand);
            bool valid = true;
            for (int j = 0; j < numHarbor; j++)
            {
                if (harborTiles[j] == val)
                {
                    valid = false;
                    break;
                }
            }
            if (valid)
            {
                harborTiles[i] = val;
            }
        }
    }
    Serial.print(F("# harbor "));
    Serial.println(numHarbor);

    bool robberAssigned = false;
    int landIndex = 0, oceanIndex = 0;

    BaseCatanState myState;

    for (GraphIterator<NodeId_t> iter(topology, catanState.id); iter.hasNext();)
    {
        NodeId_t curr = iter.next();

        topology.getAdjacent(curr, adj);

        BaseCatanState initialState;
        initialState.id = curr;

        if (ALL_LAND || adj.count == 6 || adj.count == 0)
        {
            bool isDesert = false;
            for (int i = 0; i < numDesert; i++)
            {
                if (desertTiles[i] == landIndex)
                {
                    isDesert = true;
                }
            }
            if (isDesert)
            {
                // desert tile
                initialState.landType = CatanLandType::DESERT;
                initialState.rollValue = 0;
                if (!robberAssigned)
                {
                    robberAssigned = true;
                    initialState.hasRobber = true;
                }
            }
            else
            {
                // land tile
                initialState.landType = CatanLandType::randomType();
                initialState.rollValue = randomRollValue();
            }
            landIndex++;
        }
        else
        {
            // ocean tile
            initialState.landType = CatanLandType::OCEAN;
            initialState.rollValue = 0;
            bool isHarbor = false;
            for (int i = 0; i < numHarbor; i++)
            {
                if (harborTiles[i] == oceanIndex)
                {
                    isHarbor = true;
                }
            }
            if (isHarbor)
            {
                // harbor tile
                int numPossiblePorts = 0;
                for (ListIterator<NodeId_t> adjIter(adj); adjIter.hasNext();)
                {
                    NodeId_t possiblePort = adjIter.next();
                    Serial.print(F("possible port "));
                    Serial.print(possiblePort, HEX);
                    size_t portNeighbors = topology.numAdjacent(possiblePort);
                    Serial.print(F(" adj "));
                    Serial.println(portNeighbors);
                    if (portNeighbors == 6)
                    {
                        numPossiblePorts++;
                    }
                }
                Serial.print(F("# Ports Locs "));
                Serial.println(numPossiblePorts);
                if (numPossiblePorts > 0)
                {
                    initialState.portType = CatanLandType::randomHarbor();
                    NodeId_t possiblePorts[numPossiblePorts] = {EMPTY};
                    int possiblePortIdx = 0;
                    for (ListIterator<NodeId_t> adjIter(adj); adjIter.hasNext();)
                    {
                        NodeId_t possiblePort = adjIter.next();
                        if (topology.numAdjacent(possiblePort) == 6)
                        {
                            possiblePorts[possiblePortIdx] = possiblePort;
                            possiblePortIdx++;
                        }
                    }
                    initialState.portNeighbor = possiblePorts[random(numPossiblePorts)];

                    Serial.print(F("Port Type "));
                    Serial.println(initialState.portType.toString());
                    Serial.print(F("Roll Value "));
                    Serial.println(initialState.portNeighbor, HEX);
                }
            }
            oceanIndex++;
        }

        Serial.print(F("Land Type "));
        Serial.println(initialState.landType.toString());
        Serial.print(F("Roll Value "));
        Serial.println(initialState.rollValue);

        if (curr == catanState.id)
        {
            myState = initialState;
        }
        else
        {
            SetInitialStateRequest request(initialState);
            sendSetInitialStateRequest(curr, request);
        }
    }
    SetInitialStateRequest request(myState);
    sendSetInitialStateRequest(catanState.id, request);
}

void saveState()
{
    EEPROM.put(0, catanState);
}

void loadState()
{
    EEPROM.get(0, catanState);
    newPlayer = catanState.currentPlayer;
}

int cityToNeighbor(int cityNumber)
{
    switch (cityNumber)
    {
    case 0:
    case 1:
        return -1;
    case 2:
        return 3;
    case 3:
    case 4:
        return 4;
    case 5:
        return 5;
    default:
        return -1;
    }
}

void reconcileRoadValidation(StateResponse response)
{
    if (response.placementInfo.toValidate >= NUM_ROADS)
    {
        Serial.println(F("Invalid road index"));
        return;
    }

    bool isValid = false;

    // The city behind the road
    int cityToCheck1 = (response.placementInfo.toValidate + 5) % 6;
    int neighborTile1 = cityToNeighbor(cityToCheck1);
    // The city ahead of the road
    int cityToCheck2 = response.placementInfo.toValidate % 6;
    int neighborTile2 = cityToNeighbor(cityToCheck2);

    Serial.print(F("to validate: "));
    Serial.println(response.placementInfo.toValidate);
    Serial.print(F("city 1: "));
    Serial.println(cityToCheck1);
    Serial.print(F("tile 1: "));
    Serial.println(neighborTile1);
    Serial.print(F("city 2: "));
    Serial.println(cityToCheck2);
    Serial.print(F("tile 2: "));
    Serial.println(neighborTile2);

    Serial.print(F("road check "));
    Serial.println(response.placementInfo.validateStep);

    switch (response.placementInfo.validateStep)
    {
    case 0: // check roads on same tile
        if (response.state.id == catanState.id)
        {
            if (catanState.roadOwners[(response.placementInfo.toValidate + 1) % NUM_ROADS] == response.placementInfo.playerNumber ||
                catanState.roadOwners[(response.placementInfo.toValidate - 1) % NUM_ROADS] == response.placementInfo.playerNumber)
            {
                isValid = true;
            }
            else
            {
                response.placementInfo.validateStep++;
                if (neighborTile1 < 0)
                {
                    reconcileRoadValidation(response);
                }
                else
                {
                    sendStateRequest(catanState.neighborIds[neighborTile1], response.placementInfo);
                }
            }
        }
        else
        {
            Serial.println(F("Invalid start state for road validation"));
        }
        break;
    case 1: // check previous settlement
        if (response.state.settlementOwners[cityToCheck1 % 2] == response.placementInfo.playerNumber)
        {
            isValid = true;
        }
        else
        {
            response.placementInfo.validateStep++;
            if (neighborTile2 < 0)
            {
                response.state = catanState;
                reconcileRoadValidation(response);
            }
            else
            {
                sendStateRequest(catanState.neighborIds[neighborTile2], response.placementInfo);
            }
        }
        break;
    case 2: // check next settlement
        if (response.state.settlementOwners[cityToCheck2 % 2] == response.placementInfo.playerNumber)
        {
            isValid = true;
        }
        else
        {
            response.placementInfo.validateStep++;
            sendStateRequest(catanState.neighborIds[response.placementInfo.toValidate], response.placementInfo);
        }
        break;
    case 3: // check roads on adjacent tile
        int roadToCheck1 = (response.placementInfo.toValidate + 4) % NUM_ROADS;
        int roadToCheck2 = (response.placementInfo.toValidate + 2) % NUM_ROADS;

        Serial.print(F("road 1: "));
        Serial.println(roadToCheck1);
        Serial.print(F("road 2: "));
        Serial.println(roadToCheck2);

        if (response.state.roadOwners[roadToCheck1] == response.placementInfo.playerNumber ||
            response.state.roadOwners[roadToCheck2] == response.placementInfo.playerNumber)
        {
            isValid = true;
        }
        else
        {
            Serial.println(F("Invalid road placement"));
        }
        break;
    default:
        break;
    }

    if (isValid)
    {
        setRoadOwner(SetRoadRequest(response.placementInfo.toValidate, response.placementInfo.playerNumber));
    }
}

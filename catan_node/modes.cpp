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

NodeId_t myId = EMPTY;
int btnDiscover = 0;

unsigned long previousDiscoveryMillis = 0;
bool pollDiscovery = false;
bool postDiscovery = false;

NodeId_t neighborIds[6];
Set<NodeId_t> discoveryVisited;
LinkedList<NodeId_t> discoveryQueue;

Graph<NodeId_t> topology = Graph<NodeId_t>(true, 0, EEPROM.length());

CatanState catanState;
bool playerSelectMode = false;
byte currentPlayer = 0;
bool playStarted = false;

bool sendIdRequest()
{
    Message idRequest(
        EMPTY,
        EMPTY,
        0,
        0,
        ROUTER_GET_ID,
        NULL);
    myId = 0;

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, idRequest);
        netPort.readBytes((byte *)&myId, sizeof(NodeId_t));
    }
    else
    {
        disp.setChars("ID req Failure ");
        return false;
    }

    return true;
}

bool sendDiscoveryRequest()
{
    if (myId == EMPTY && !sendIdRequest())
    {
        return false;
    }
    Message discoveryRequest(
        myId,
        myId,
        0,
        0,
        ROUTER_START_DISCOVERY,
        NULL);

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, discoveryRequest);
    }
    else
    {
        disp.setChars("D req Failure ");
        return false;
    }
    return true;
}

bool sendDiscoveryStatsRequest()
{
    Message discoveryStatsRequest(
        myId,
        myId,
        0,
        0,
        ROUTER_GET_DISCOVERY_STATUS,
        NULL);

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, discoveryStatsRequest);
    }
    else
    {
        disp.setChars("D stat Failure ");
        return false;
    }
    return true;
}

bool sendNeighborRequest(NodeId_t destination, bool useCache)
{
    Serial.print(F("Request neighbors "));
    Serial.println(destination, HEX);
    Serial.print(F("Use Cache "));
    Serial.println(useCache);
    Message neighborRequest(
        myId,
        destination,
        0,
        useCache ? ROUTER_USE_CACHE : 0,
        ROUTER_GET_NEIGHBORS,
        NULL);

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, neighborRequest);
    }
    else
    {
        disp.setChars("N req Failure ");
        return false;
    }
    return true;
}

void handleDiscoveryStatsResponse(const Message &message)
{
    Serial.println(F("handle disc stat resp"));
    DiscoveryStats *stats = (DiscoveryStats *)message.getBody();
    sprintf(displayMessage, "D: %d, N: %d, E: %d    ", stats->discoveryDone, stats->numNodes, stats->numEdges);
    disp.setChars(displayMessage);
    Serial.println(displayMessage);
    if (stats->discoveryDone)
    {
        pollDiscovery = false;
        postDiscovery = true;
        sendNeighborRequest(myId, true);
    }
}

void handleNodeResponseNetworkTest(const Message &message)
{
    NodeId_t *nodes = ((NodeId_t *)(message.getBody()));
    if (!postDiscovery)
    {
        __int24 ledColors[leds.getNumLeds()] = {BLACK};
        sprintf(
            displayMessage,
            "Neighbors [%04X, %04X, %04X, %04X, %04X, %04X] ",
            nodes[1],
            nodes[2],
            nodes[3],
            nodes[4],
            nodes[5],
            nodes[6]);

        disp.setChars(displayMessage);
        Serial.println(displayMessage);
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
            if (message.getSource() == myId)
            {
                neighborIds[i] = id;
            }
        }
        leds.setState(ledColors);
    }
    else
    {
        int numNodes = message.getPayloadSize() / sizeof(NodeId_t);
        Serial.print(F("Num nodes "));
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
            Serial.println(F("Post Discovery Complete"));
            discoveryQueue.purge();
            discoveryVisited.purge();
            postDiscovery = false;
            sendNeighborRequest(myId, true);
        }
        else
        {
            NodeId_t nextNode = discoveryQueue.popFront();

            WakeNodeMessage command;
            Message msg(
                myId,
                nextNode,
                sizeof(WakeNodeMessage),
                0,
                0,
                (byte *)&command);

            Serial.print(F("Waking next node "));
            Serial.println(nextNode, HEX);

            if (msg.getDest() != EMPTY)
            {
                if (ackWait(&netPort, MAX_NET_RETRIES))
                {
                    writeMessage(&netPort, msg);
                }
                else
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
    for (int i = CatanLandType::OCEAN; i < CatanLandType::NONE; i++)
    {
        totalWeight += static_cast<CatanLandType>(i).toWeight(includeDesert);
    }
    Serial.print(F("Total weight "));
    Serial.println(totalWeight);

    int rnd = random(totalWeight);
    for (int i = CatanLandType::OCEAN; i < CatanLandType::NONE; i++)
    {
        if (rnd < static_cast<CatanLandType>(i).toWeight(includeDesert))
        {
            return static_cast<CatanLandType>(i);
        }
        rnd -= static_cast<CatanLandType>(i).toWeight(includeDesert);
    }

    return CatanLandType::NONE;
}

CatanLandType CatanLandType::randomHarbor()
{
    int totalWeight = 0;
    for (int i = CatanLandType::OCEAN; i < CatanLandType::NONE; i++)
    {
        totalWeight += static_cast<CatanLandType>(i).toHarborWeight();
    }
    Serial.print(F("Total weight "));
    Serial.println(totalWeight);

    int rnd = random(totalWeight);
    for (int i = CatanLandType::OCEAN; i < CatanLandType::NONE; i++)
    {
        if (rnd < static_cast<CatanLandType>(i).toHarborWeight())
        {
            return static_cast<CatanLandType>(i);
        }
        rnd -= static_cast<CatanLandType>(i).toHarborWeight();
    }

    return CatanLandType::NONE;
}

int CatanLandType::numDessertTiles(int numLandTiles)
{
    return numLandTiles / 30 + 1;
}

int CatanLandType::numHarborTiles(int numOceanTiles)
{
    return min(numOceanTiles, 11);
}

void handleNodeResponseCatan(const Message &message)
{
    NodeId_t *nodes = ((NodeId_t *)(message.getBody()));

    if (!playStarted)
    {
        int numNodes = message.getPayloadSize() / sizeof(NodeId_t);
        Serial.print(F("Num nodes "));
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
            Serial.println(F("Post Discovery Complete"));
            discoveryQueue.purge();

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
            neighborIds[i] = id;
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
                validation.playerNumber = currentPlayer;
                reconcileRoadValidation(StateResponse(validation, catanState));
            }
            else if (catanState.roadOwners[i] == currentPlayer)
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
                validation.playerNumber = currentPlayer;
                reconcileSettlementValidation(StateResponse(validation, catanState));
            }
            else if (catanState.settlementOwners[i] == currentPlayer)
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
            currentPlayer = i;
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
        ledColors[LED_LAND] = getPlayerColor(currentPlayer);
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
            Serial.print(F("harbor type "));
            Serial.println(catanState.portType.toString());
            Serial.print(F("harbor port "));
            Serial.println(catanState.portNeighbor, HEX);
            for (int i = 0; i < 6; i++)
            {
                if (neighborIds[i] == catanState.portNeighbor)
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
    if (val < MIN_ROLL)
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
            neighborIds[request.roadNumber],
            sizeof(SetRoadRequest),
            0,
            0,
            (byte *)&command);

        Serial.print(F("updateNeighbor: "));
        Serial.println(updateNeighbor);

        if (msg.getDest() != EMPTY)
        {
            if (ackWait(&netPort, MAX_NET_RETRIES))
            {
                writeMessage(&netPort, msg);
            }
            else
            {
                Serial.println(F("Fail"));
            }
        }
    }
}

void setInitialState(NodeId_t node, SetInitialStateRequest request)
{
    catanState.landType = request.initialState.landType;
    catanState.rollValue = request.initialState.rollValue;
    catanState.hasRobber = request.initialState.hasRobber;
    catanState.portType = request.initialState.portType;
    catanState.portNeighbor = request.initialState.portNeighbor;

    catanState.controllerId = node;

    if (catanState.hasRobber)
    {
        setTileValue(0xFF);
    }
    else if (catanState.portType != CatanLandType::NONE)
    {
        if (catanState.portType == CatanLandType::DESERT)
        {
            disp.setChars("3/1  ");
        }
        else
        {
            sprintf(displayMessage, "2/1 %s ", catanState.portType.toString());
            disp.setChars(displayMessage);
        }
    }
    else
    {
        setTileValue(catanState.rollValue);
    }

    playStarted = true;

    sendIdRequest();
    catanState.id = myId;

    Serial.print(F("ID: "));
    Serial.println(catanState.id, HEX);

    sendNeighborRequest(myId, true);
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

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, msg);
    }
    else
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

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, msg);
    }
    else
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

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, msg);
    }
    else
    {
        Serial.println(F("Fail"));
    }
}

void sendSetCurrentPlayerRequest()
{
    SetCurrentPlayerRequest request(currentPlayer);
    Message msg(
        catanState.id,
        catanState.id,
        sizeof(SetCurrentPlayerRequest),
        ROUTER_BROADCAST,
        0,
        (byte *)&request);

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, msg);
    }
    else
    {
        Serial.println(F("Fail"));
    }
}

void sendClearRobberRequest()
{
    ClearRobberRequest request(myId);
    Message msg(
        catanState.id,
        catanState.id,
        sizeof(ClearRobberRequest),
        ROUTER_BROADCAST,
        0,
        (byte *)&request);

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, msg);
    }
    else
    {
        Serial.println(F("Fail"));
    }
}

void setCurrentPlayer(SetCurrentPlayerRequest request)
{
    currentPlayer = request.playerNumber;
    __int24 ledColors[leds.getNumLeds()] = {BLACK};
    ledColors[LED_LAND] = getPlayerColor(currentPlayer);
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

    byte neighboringSettlement = (response.placementInfo.toValidate + 1) % NUM_SETTLEMENTS;
    Serial.print(F("validate settlement "));
    Serial.println(response.placementInfo.toValidate);
    if (response.state.id == catanState.id ||
        response.state.id == neighborIds[response.placementInfo.toValidate] ||
        response.state.id == neighborIds[response.placementInfo.toValidate + 1])
    {
        response.placementInfo.onLand = response.placementInfo.onLand || response.state.landType != CatanLandType::OCEAN;
    }

    if (response.placementInfo.validateStep == 3 || response.state.settlementOwners[neighboringSettlement] == UNOWNED)
    {
        bool hasNextTile = false;
        byte nextTileIdx;
        for (; response.placementInfo.validateStep < (3 - response.placementInfo.toValidate) && !hasNextTile; response.placementInfo.validateStep++)
        {
            Serial.print(F("step "));
            Serial.println(response.placementInfo.validateStep);
            nextTileIdx = (response.placementInfo.validateStep + (response.placementInfo.toValidate * 2) + 5) % 6;
            Serial.print(F("nextTileIdx "));
            Serial.println(nextTileIdx);
            Serial.print(F("nextTileId "));
            Serial.println(neighborIds[nextTileIdx], HEX);
            if (neighborIds[nextTileIdx] != EMPTY)
            {
                hasNextTile = true;
            }
        }

        if (hasNextTile)
        {
            Serial.println(F("request next tile"));
            sendStateRequest(neighborIds[nextTileIdx], response.placementInfo);
        }
        else if (!response.placementInfo.onLand)
        {
            Serial.println(F("cannot place settlement on ocean"));
        }
        else
        {
            Serial.println(F("Settlement place approved"));
            catanState.settlementOwners[response.placementInfo.toValidate] = response.placementInfo.playerNumber;
        }
    }
    else
    {
        Serial.print(F("Invalid settlment neighbor "));
        Serial.println(response.state.settlementOwners[neighboringSettlement]);
        Serial.print(F("step "));
        Serial.println(response.placementInfo.validateStep);
    }
}

void setupBoard()
{
    Serial.println(F("Setup board"));

    int numLand = 0;
    for (GraphIterator<NodeId_t> iter(topology, myId); iter.hasNext();)
    {
        NodeId_t curr = iter.next();
        Set<NodeId_t> adj;
        topology.getAdjacent(curr, adj);
        if (ALL_LAND || adj.count == 0 || adj.count == 6)
        {
            numLand++;
        }
    }

    // get indicies for desert tiles
    const int numDesert = CatanLandType::numDessertTiles(numLand);
    Set<int> desertTiles;
    while (desertTiles.count < numDesert)
    {
        desertTiles.pushBack(random(numLand));
    }
    Serial.print(F("Num desert "));
    Serial.println(numDesert);

    // get indicies for harbor tiles
    const int numHarbor = CatanLandType::numHarborTiles(topology.numNodes() - numLand);
    Set<int> harborTiles;
    while (harborTiles.count < numHarbor)
    {
        harborTiles.pushBack(random(topology.numNodes() - numLand));
    }
    Serial.print(F("Num harbor "));
    Serial.println(numHarbor);

    bool robberAssigned = false;
    int landIndex = 0, oceanIndex = 0;

    BaseCatanState myState;

    for (GraphIterator<NodeId_t> iter(topology, myId); iter.hasNext();)
    {
        NodeId_t curr = iter.next();

        Set<NodeId_t> adj;
        topology.getAdjacent(curr, adj);

        BaseCatanState initialState;

        if (ALL_LAND || adj.count == 6 || adj.count == 0)
        {
            if (desertTiles.contains(landIndex))
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
                initialState.rollValue = random(NUM_DICE, NUM_DICE * DIE_SIDES);
            }
            landIndex++;
        }
        else
        {
            // ocean tile
            initialState.landType = CatanLandType::OCEAN;
            if (harborTiles.contains(oceanIndex))
            {
                // harbor tile
            }
        }

        Serial.print(F("Land Type "));
        Serial.println(initialState.landType.toString());
        Serial.print(F("Roll Value "));
        Serial.println(initialState.rollValue);

        if (curr == myId)
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
    sendSetInitialStateRequest(myId, request);
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
                    sendStateRequest(neighborIds[neighborTile1], response.placementInfo);
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
                sendStateRequest(neighborIds[neighborTile2], response.placementInfo);
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
            sendStateRequest(neighborIds[response.placementInfo.toValidate], response.placementInfo);
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

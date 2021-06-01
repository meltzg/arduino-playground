#include "modes.h"

const byte Mode::EDGE_LED_POS[6] = {0, 3, 6, 7, 8, 9};
const byte Mode::EDGE_BTN_POS[6] = {0, 2, 4, 5, 6, 7};
const byte Mode::CORNER_LED_POS[2][2] = {{1, 2}, {4, 5}};
const byte Mode::CORNER_BTN_POS[2] = {1, 3};

void ComponentTestMode::init()
{
    disp.setRenderChars(false);
}

void ComponentTestMode::processState(unsigned long currentMillis, uint16_t state)
{
    static int colorOffset = 0;
    static int segmentOffset = 0;

    if (currentMillis - previousMillis > 500)
    {
        disp.registerWrite(~(1 << (segmentOffset++ % 16)));

        previousMillis = currentMillis;
    }
    if (currentMillis - previousMillisLeds > 100)
    {
        __int24 ledColors[leds.getNumLeds()] = {BLACK};
        for (int i = 0; i < leds.getNumLeds(); i++)
        {
            if (((1 << i) & state) > 0)
            {
                continue;
            }
            ledColors[i] = RAINBOW[(i + colorOffset) % 6];
        }
        colorOffset++;
        leds.setState(ledColors);
        previousMillisLeds = currentMillis;
    }
}

void NetworkTestMode::init()
{
    disp.setRenderChars(true);
    __int24 ledColors[leds.getNumLeds()];
    for (int i = 0; i < leds.getNumLeds(); i++)
    {
        ledColors[i] = BLACK;
    }
    leds.setState(ledColors);
    disp.setChars("Start ");
    for (int i = 0; i < 6; i++)
    {
        neighborIds[i] = EMPTY;
    }
}

void NetworkTestMode::processState(unsigned long currentMillis, uint16_t state)
{
    if (state != previousState)
    {
        if (((previousState >> BTN_ID) & 1) && ((state >> BTN_ID) & 1) == 0)
        {
            handleIdRequest();
        }
        else if (((previousState >> BTN_NEIGHBORS) & 1) && ((state >> BTN_NEIGHBORS) & 1) == 0)
        {
            handleNeighborRequest(myId);
        }
        else if (((previousState >> BTN_DISCOVER) & 1) && ((state >> BTN_DISCOVER) & 1) == 0)
        {
            handleDiscoveryRequest();
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {
                if (((previousState >> EDGE_BTN_POS[i]) & 1) && ((state >> EDGE_BTN_POS[i]) & 1) == 0 && neighborIds[i] != EMPTY)
                {
                    handleNeighborRequest(neighborIds[i]);
                    break;
                }
            }
        }

        previousState = state;
    }
}

void NetworkTestMode::processMessage(const Message &message)
{
    if (message.sysCommand & ROUTER_ADD_NODE)
    {
        handleNodeResponse(message);
    }
}

void NetworkTestMode::handleNodeResponse(const Message &message)
{
    __int24 ledColors[leds.getNumLeds()] = {BLACK};
    sprintf(
        displayMessage,
        "Neighbors [%04X, %04X, %04X, %04X, %04X, %04X] ",
        ((NodeId_t *)(message.body))[1],
        ((NodeId_t *)(message.body))[2],
        ((NodeId_t *)(message.body))[3],
        ((NodeId_t *)(message.body))[4],
        ((NodeId_t *)(message.body))[5],
        ((NodeId_t *)(message.body))[6]);

    disp.setChars(displayMessage);
    Serial.println(displayMessage);
    for (int i = 0; i < 6; i++)
    {
        NodeId_t id = ((NodeId_t *)(message.body))[i + 1];
        if (id == EMPTY)
        {
            ledColors[EDGE_LED_POS[i]] = RED;
        }
        else
        {
            ledColors[EDGE_LED_POS[i]] = GREEN;
        }
        if (message.source == myId)
        {
            neighborIds[i] = id;
        }
    }
    leds.setState(ledColors);
}

void NetworkTestMode::handleIdRequest()
{
    Message idRequest;
    idRequest.source = EMPTY;
    idRequest.dest = EMPTY;
    idRequest.payloadSize = 0;
    idRequest.sysCommand = ROUTER_GET_ID;
    idRequest.body = NULL;
    myId = 0;

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, idRequest);
        netPort.readBytes((byte *)&myId, sizeof(NodeId_t));
    }
    else
    {
        disp.setChars("Failure ");
        return;
    }

    sprintf(displayMessage, "My ID: %04X ", myId);
    disp.setChars(displayMessage);

    Serial.println(displayMessage);
}

void NetworkTestMode::handleNeighborRequest(NodeId_t destination)
{
    Message neighborRequest;
    neighborRequest.source = myId;
    neighborRequest.dest = destination;
    neighborRequest.payloadSize = 0;
    neighborRequest.sysCommand = ROUTER_GET_NEIGHBORS;
    neighborRequest.body = NULL;

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, neighborRequest);
    }
    else
    {
        disp.setChars("Failure ");
    }
}

CatanLandType CatanLandType::randomType()
{
    int totalWeight = 0;
    for (int i = CatanLandType::OCEAN; i < CatanLandType::NONE; i++)
    {
        totalWeight += static_cast<CatanLandType>(i).toWeight();
    }
    Serial.print(F("Total weight "));
    Serial.println(totalWeight);

    int rnd = random(totalWeight);
    for (int i = CatanLandType::OCEAN; i < CatanLandType::NONE; i++)
    {
        if (rnd < static_cast<CatanLandType>(i).toWeight())
        {
            return static_cast<CatanLandType>(i);
        }
        rnd -= static_cast<CatanLandType>(i).toWeight();
    }

    return CatanLandType::NONE;
}

void CatanMode::init()
{
    randomSeed(analogRead(SEED_PIN));
    disp.setRenderChars(true);
    if (!playStarted)
    {
        disp.setChars("Catan ");
        for (int i = 0; i < 6; i++)
        {
            neighborIds[i] = EMPTY;
        }

        for (int i = 0; i < NUM_ROADS; i++)
        {
            catanState.roadOwners[i] = UNOWNED;
        }
        for (int i = 0; i < NUM_SETTLEMENTS; i++)
        {
            catanState.settlementOwners[i] = UNOWNED;
        }
    }
    else
    {
        setTileValue(catanState.hasRobber ? 0xFF : catanState.rollValue);
    }
}

void CatanMode::processState(unsigned long currentMillis, uint16_t state)
{
    bool hadMessage = false;
    if (hasIncoming(&netPort))
    {
        hadMessage = true;
        Message message = readMessage(&netPort);
        processMessage(message);
        delete[] message.body;
        message.body = NULL;
    }

    if (currentMillis - previousMillis > 75)
    {
        bool skipRobber = false;

        if (playStarted && !playerSelectMode && btns.getOnDuration(BTN_LAND) >= PLAYER_SELECT_DELAY)
        {
            playerSelectMode = true;
            disp.setChars(catanState.landType.toString());
            updateCurrentPlayer(state);
        }
        else if (playerSelectMode && btns.getOnDuration(BTN_LAND) == 0)
        {
            playerSelectMode = false;
            setTileValue(catanState.rollValue);
            skipRobber = true;
        }
        if (!playStarted && ((previousState >> BTN_LAND) & 1) && ((state >> BTN_LAND) & 1) == 0)
        {
            setupGame();
        }
        else if (state != previousState)
        {
            if (!playerSelectMode)
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
        }

        renderState();
        previousState = state;
        previousMillis = currentMillis;
    }
}

void CatanMode::updateRoads(uint16_t state)
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

void CatanMode::updateSettlements(uint16_t state)
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

void CatanMode::updateRobber(uint16_t state)
{
    if (catanState.landType == CatanLandType::OCEAN)
    {
        catanState.hasRobber = false;
        return;
    }

    if (((previousState >> BTN_LAND) & 1) && ((state >> BTN_LAND) & 1) == 0)
    {
        if (catanState.hasRobber)
        {
            catanState.hasRobber = false;
            setTileValue(catanState.rollValue);
        }
        else
        {
            catanState.hasRobber = true;
            setTileValue(0xFF);
        }
    }
}

void CatanMode::updateCurrentPlayer(uint16_t state)
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

void CatanMode::renderState()
{
    __int24 ledColors[leds.getNumLeds()] = {BLACK};

    if (playerSelectMode)
    {
        for (int i = 0; i < NUM_ROADS; i++)
        {
            byte ledPos = EDGE_LED_POS[i];
            ledColors[ledPos] = getPlayerColoer(i);
        }
        for (int i = 0; i < NUM_SETTLEMENTS; i++)
        {
            byte *ledPos = CORNER_LED_POS[i];
            ledColors[ledPos[0]] = BLACK;
            ledColors[ledPos[1]] = BLACK;
        }
        ledColors[LED_LAND] = getPlayerColoer(currentPlayer);
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
                ledColors[ledPos] = getPlayerColoer(catanState.roadOwners[i]);
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
                ledColors[ledPos[0]] = getPlayerColoer(catanState.settlementOwners[i]);
                if (catanState.isCity[i])
                {
                    ledColors[ledPos[1]] = getPlayerColoer(catanState.settlementOwners[i]);
                }
            }
        }
        ledColors[LED_LAND] = catanState.landType.toGRB();
    }

    leds.setState(ledColors);
}

void CatanMode::setupGame()
{
    Serial.println(F("setup"));
    if (setupStage == 0)
    {
        Message idRequest;
        idRequest.source = EMPTY;
        idRequest.dest = EMPTY;
        idRequest.payloadSize = 0;
        idRequest.sysCommand = ROUTER_GET_ID;
        idRequest.body = NULL;

        if (ackWait(&netPort, MAX_NET_RETRIES))
        {
            writeMessage(&netPort, idRequest);
            netPort.readBytes((byte *)&catanState.id, sizeof(NodeId_t));
        }
        else
        {
            Serial.println(F("Fail"));
            return;
        }

        Serial.print(F("ID: "));
        Serial.println(catanState.id, HEX);

        Message neighborRequest;
        neighborRequest.source = catanState.id;
        neighborRequest.dest = catanState.id;
        neighborRequest.payloadSize = 0;
        neighborRequest.sysCommand = ROUTER_GET_NEIGHBORS;
        neighborRequest.body = NULL;

        if (ackWait(&netPort, MAX_NET_RETRIES))
        {
            writeMessage(&netPort, neighborRequest);
        }
        else
        {
            Serial.println(F("Fail"));
        }
    }
    else
    {
        int numEmpty = 0;
        for (int i = 0; i < 6; i++)
        {
            if (neighborIds[i] == EMPTY)
            {
                numEmpty++;
            }
        }
        if (numEmpty == 0 || numEmpty == 6 || ALL_LAND)
        {
            catanState.landType = CatanLandType::randomType();
        }
        else
        {
            catanState.landType = CatanLandType::OCEAN;
        }

        if (catanState.landType == CatanLandType::OCEAN || catanState.landType == CatanLandType::DESERT)
        {
            catanState.rollValue = 0;
        }
        else
        {
            catanState.rollValue = random(NUM_DICE, NUM_DICE * DIE_SIDES);
        }

        setTileValue(catanState.rollValue);

        playStarted = true;
    }
}

void CatanMode::setTileValue(byte val)
{
    if (val < MIN_ROLL)
    {
        sprintf(displayValue, "  ");
    }
    else if (val > MAX_ROLL)
    {
        sprintf(displayValue, "Robber ");
    }
    else
    {
        sprintf(displayValue, "%02d", val);
    }
    disp.setChars(displayValue);
}

void CatanMode::processMessage(const Message &message)
{
    if (message.sysCommand)
    {
        if (message.sysCommand & ROUTER_ADD_NODE && message.source == catanState.id)
            advanceSetupStage(CATAN_SETUP_NEIGHBORS);
        for (int i = 0; i < 6; i++)
        {
            NodeId_t id = ((NodeId_t *)(message.body))[i + 1];
            neighborIds[i] = id;
            Serial.println(id, HEX);
        }
    }
    else
    {
        CatanMessage *command = (CatanMessage *)message.body;
        switch (command->command)
        {
        case SET_ROAD:
            setRoadOwner(*(SetRoadRequest *)command, false);
            break;
        case GET_STATE:
            sendStateResponse(message.source, ((GetStateRequest *)command)->placementInfo);
            break;
        case STATE_RESPONSE:
            reconcileStateResponse(*(StateResponse *)command);
        default:
            break;
        }
    }
    if (!playStarted)
    {
        setupGame();
    }
}

void CatanMode::advanceSetupStage(byte stage)
{
    if (setupStage <= stage)
    {
        ++setupStage;
    }
}

__int24 CatanMode::getPlayerColoer(byte playerNumber)
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

void CatanMode::setRoadOwner(SetRoadRequest request, bool updateNeighbor = true)
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
        Message msg;
        msg.source = catanState.id;
        msg.dest = neighborIds[request.roadNumber];
        msg.body = (byte *)&command;
        msg.payloadSize = sizeof(SetRoadRequest);

        Serial.print(F("updateNeighbor: "));
        Serial.println(updateNeighbor);

        if (msg.dest != EMPTY)
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

void CatanMode::sendStateRequest(NodeId_t node, PlacementValidationInfo placementInfo)
{
    GetStateRequest request(placementInfo);
    Message msg;
    msg.source = catanState.id;
    msg.dest = node;
    msg.body = (byte *)&request;
    msg.payloadSize = sizeof(GetStateRequest);

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, msg);
    }
    else
    {
        Serial.println(F("Fail"));
    }
}

void CatanMode::sendStateResponse(NodeId_t node, PlacementValidationInfo placementInfo)
{
    StateResponse response(placementInfo, catanState);
    Message msg;
    msg.source = catanState.id;
    msg.dest = node;
    msg.body = (byte *)&response;
    msg.payloadSize = sizeof(StateResponse);

    if (ackWait(&netPort, MAX_NET_RETRIES))
    {
        writeMessage(&netPort, msg);
    }
    else
    {
        Serial.println(F("Fail"));
    }
}

void CatanMode::reconcileStateResponse(StateResponse response)
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

void CatanMode::reconcileSettlementValidation(StateResponse response)
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

void CatanMode::reconcileRoadValidation(StateResponse response)
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
            if (neighborTile1 < 0)
            {
                reconcileRoadValidation(response);
            }
            else
            {
                sendStateRequest(neighborIds[neighborTile1], response.placementInfo);
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

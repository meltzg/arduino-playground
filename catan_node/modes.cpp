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
                setRoadOwner(SetRoadRequest(i, currentPlayer));
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
                catanState.settlementOwners[i] = currentPlayer;
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

            if (catanState.roadOwners[i] == UNOWNED)
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
            netPort.readBytes((byte *)&myId, sizeof(NodeId_t));
        }
        else
        {
            Serial.println(F("Fail"));
            return;
        }

        Serial.print(F("ID: "));
        Serial.println(myId, HEX);

        Message neighborRequest;
        neighborRequest.source = myId;
        neighborRequest.dest = myId;
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
            catanState.landType = CatanLandType(random(CatanLandType::DESERT, CatanLandType::WHEAT + 1));
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
        if (message.sysCommand & ROUTER_ADD_NODE && message.source == myId)
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
        msg.source = myId;
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
    msg.source = myId;
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

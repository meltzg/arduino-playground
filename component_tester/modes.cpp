#include "modes.h"

const byte Mode::EDGE_LED_POS[6] = {0, 3, 6, 7, 8, 9};
const byte Mode::EDGE_BTN_POS[6] = {0, 2, 4, 5, 6, 7};
const byte Mode::CORNER_LED_POS[2][2] = {{1, 2}, {4, 5}};
const byte Mode::CORNER_BTN_POS[2] = {1, 3};

void ComponentTestMode::init()
{
    disp.setRenderChars(false);
}

void ComponentTestMode::process(unsigned long currentMillis)
{
    uint16_t btnState = btns.getState();

    static int colorOffset = 0;

    if (currentMillis - previousMillis > 500)
    {
        __int24 ledColors[leds.getNumLeds()] = {BLACK};
        for (int i = 0; i < leds.getNumLeds(); i++)
        {
            if (((1 << i) & btnState) > 0)
            {
                continue;
            }
            ledColors[i] = RAINBOW[(i + colorOffset) % 6];
        }
        colorOffset++;
        leds.setState(ledColors);
        disp.registerWrite(~(1 << (colorOffset % 16)));

        previousMillis = currentMillis;
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

void NetworkTestMode::process(unsigned long currentMillis)
{
    if (hasIncoming(&netPort))
    {
        Serial.println("recieving message");
        Message message = readMessage(&netPort);
        processMessage(&netPort, message);
        delete[] message.body;
        message.body = NULL;
    }

    uint16_t btnState = btns.getState();

    if (btnState != previousState)
    {
        if (((previousState >> BTN_ID) & 1) && ((btnState >> BTN_ID) & 1) == 0)
        {
            handleIdRequest();
        }
        else if (((previousState >> BTN_NEIGHBORS) & 1) && ((btnState >> BTN_NEIGHBORS) & 1) == 0)
        {
            handleNeighborRequest(myId);
        }
        else if (((previousState >> BTN_DISCOVER) & 1) && ((btnState >> BTN_DISCOVER) & 1) == 0)
        {
            handleDiscoveryRequest();
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {
                if (((previousState >> EDGE_BTN_POS[i]) & 1) && ((btnState >> EDGE_BTN_POS[i]) & 1) == 0 && neighborIds[i] != EMPTY)
                {
                    handleNeighborRequest(neighborIds[i]);
                    break;
                }
            }
        }

        previousState = btnState;
    }
}

void NetworkTestMode::processMessage(Stream *srcPort, const Message &message)
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
    int maxRetries = 1000;
    Message idRequest;
    idRequest.source = EMPTY;
    idRequest.dest = EMPTY;
    idRequest.payloadSize = 0;
    idRequest.sysCommand = ROUTER_GET_ID;
    idRequest.body = NULL;
    myId = 0;

    if (ackWait(&netPort, maxRetries))
    {
        writeMessage(&netPort, idRequest);
        netPort.readBytes((byte *)&myId, sizeof(NodeId_t));
    }
    else
    {
        disp.setChars("Failure ");
        Serial.println("Failure");
        return;
    }

    sprintf(displayMessage, "My ID: %04X ", myId);
    disp.setChars(displayMessage);

    Serial.println(displayMessage);
}

void NetworkTestMode::handleNeighborRequest(NodeId_t destination)
{
    int maxRetries = 1000;
    Message neighborRequest;
    neighborRequest.source = myId;
    neighborRequest.dest = destination;
    neighborRequest.payloadSize = 0;
    neighborRequest.sysCommand = ROUTER_GET_NEIGHBORS;
    neighborRequest.body = NULL;

    if (ackWait(&netPort, maxRetries))
    {
        writeMessage(&netPort, neighborRequest);
    }
    else
    {
        disp.setChars("Failure ");
        Serial.println("Failure");
    }
}

const __int24 CatanMode::PLAYER_COLORS[6] = {RED, ORANGE, GREEN, BLUE, PURPLE, WHITE};
const __int24 CatanMode::LAND_COLORS[8] = {OCEAN, DESERT, BRICK, SHEEP, WOOD, STONE, WHEAT, OCEAN};

void CatanMode::init()
{
    randomSeed(analogRead(SEED_PIN));
    disp.setRenderChars(true);
    if (!playStarted)
    {
        if (borderColors == NULL)
        {
            borderColors = new __int24[leds.getNumLeds()];
        }
        for (int i = 0; i < leds.getNumLeds(); i++)
        {
            borderColors[i] = BLACK;
        }
        leds.setState(borderColors);
        disp.setChars("Catan ");
        for (int i = 0; i < 6; i++)
        {
            neighborIds[i] = EMPTY;
        }

        for (int i = 0; i < NUM_ROADS; i++)
        {
            roadOwners[i] = UNOWNED;
        }
        for (int i = 0; i < NUM_SETTLEMENTS; i++)
        {
            settlementOwners[i] = UNOWNED;
        }
    }
}

void CatanMode::process(unsigned long currentMillis)
{
    bool skipRobber = false;
    uint16_t state = btns.getState();

    if (playStarted && !playerSelectMode && btns.getOnDuration(BTN_LAND) >= PLAYER_SELECT_DELAY)
    {
        playerSelectMode = true;
        updateCurrentPlayer(state);
    }
    else if (playerSelectMode && btns.getOnDuration(BTN_LAND) == 0)
    {
        playerSelectMode = false;
        skipRobber = true;
        borderColors[LED_LAND] = landType;
    }
    if (state != previousState)
    {
        if (!playStarted)
        {
            setupGame();
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
    }
    leds.setState(borderColors);
    previousState = state;
}

void CatanMode::updateRoads(uint16_t state)
{
    Serial.println("update roads");
    for (int i = 0; i < NUM_ROADS; i++)
    {
        byte ledPos = EDGE_LED_POS[i];
        byte btnPos = EDGE_BTN_POS[i];

        if (((previousState >> btnPos) & 1) && ((state >> btnPos) & 1) == 0)
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
        char message[10] = { 0 };
        sprintf(message, "led %d: ", ledPos);
        Serial.print(message);
        
        if (roadOwners[i] == UNOWNED)
        {
            Serial.println("unowned");
            borderColors[ledPos] = BLACK;
        }
        else
        {
            Serial.println((int) PLAYER_COLORS[roadOwners[i]], HEX);
            borderColors[ledPos] = PLAYER_COLORS[roadOwners[i]];
        }
    }
    leds.setState(borderColors);
}

void CatanMode::updateSettlements(uint16_t state) {}

void CatanMode::updateRobber(uint16_t state) {}

void CatanMode::updateCurrentPlayer(uint16_t state) {}

void CatanMode::setupGame() {
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
        netPort.readBytes((byte *)&myId, sizeof(NodeId_t));
    }
    else
    {
        Serial.println("Failure");
        return;
    }

    Serial.print("My ID: ");
    Serial.println(myId, HEX);

    playStarted = true;
}

void CatanMode::setTileValue(byte val) {}

#include "modes.h"

// State information
byte modeIdx = MODE_CATAN;

void setup()
{
    Serial.begin(9600);
    netPort.begin(SOFT_BAUD);

    __int24 ledColors[NUM_LEDS] = {BLACK};
    leds.setState(ledColors);
    selectMode();
    delay(3000);
}

void loop()
{
    unsigned long currentMillis = millis();
    disp.render(currentMillis);
    leds.render(currentMillis);
    btns.render(currentMillis);

    uint16_t btnState = btns.getState();

    if (((previousState >> BTN_CENTER) & 1) && ((btnState >> BTN_CENTER) & 1) == 0)
    {
        Serial.print(F("Old Mode "));
        Serial.println(modeIdx);

        modeIdx++;
        modeIdx %= NUM_MODES;

        Serial.print(F("New Mode "));
        Serial.println(modeIdx);

        selectMode();
    }

    Message message = readMessage(&netPort);
    if (message.isValid())
    {
        if (!message.getSysCommand() && message.getPayloadSize() > 0)
        {
            ModeMessage *command = (ModeMessage *)message.getBody();
            Serial.print(F("Current Mode ID "));
            Serial.println(modeIdx);
            Serial.print(F("Message Mode ID "));
            Serial.println(command->modeId);
            if (command->modeId != modeIdx)
            {
                modeIdx = command->modeId;
                selectMode();
            }
        }

        processMessage(message);
        message.free();
    }
    processState(currentMillis, btnState);
}

void selectMode()
{
    if (modeIdx == MODE_COMPONENT_TEST)
    {
        Serial.println(F("Init component"));
        disp.setRenderChars(false);
    }
    else if (modeIdx == MODE_NETWORK_TEST)
    {
        Serial.println(F("Init network test"));
        btnDiscover = BTN_DISCOVER;
        disp.setRenderChars(true);
        __int24 ledColors[leds.getNumLeds()];
        for (int i = 0; i < leds.getNumLeds(); i++)
        {
            ledColors[i] = BLACK;
        }
        leds.setState(ledColors);
        disp.setChars("Start ");
    }
    else if (modeIdx == MODE_CATAN)
    {
        Serial.println(F("Init catan"));
        btnDiscover = BTN_LAND;
        randomSeed(analogRead(SEED_PIN));
        disp.setRenderChars(true);
        if (!playStarted)
        {
            disp.setChars("Catan ");
            for (int i = 0; i < 6; i++)
            {
                catanState.neighborIds[i] = EMPTY;
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
}

void processState(unsigned long currentMillis, uint16_t state)
{
    bool doDiscoveryProcessing = false;

    if (modeIdx == MODE_COMPONENT_TEST)
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
                    Serial.print(F("btn "));
                    Serial.println(i);
                    continue;
                }
                ledColors[i] = RAINBOW[(i + colorOffset) % 6];
            }
            colorOffset++;
            leds.setState(ledColors);
            previousMillisLeds = currentMillis;
        }
        previousState = state;
    }
    else if (modeIdx == MODE_NETWORK_TEST)
    {
        if (state != previousState)
        {
            if (((previousState >> BTN_ID) & 1) && ((state >> BTN_ID) & 1) == 0)
            {
                sendIdRequest();
            }
            else if (((previousState >> BTN_NEIGHBORS) & 1) && ((state >> BTN_NEIGHBORS) & 1) == 0)
            {
                sendNeighborRequest(catanState.id);
            }
            else
            {
                for (int i = 0; i < 6; i++)
                {
                    if (((previousState >> EDGE_BTN_POS[i]) & 1) && ((state >> EDGE_BTN_POS[i]) & 1) == 0 && catanState.neighborIds[i] != EMPTY)
                    {
                        sendNeighborRequest(catanState.neighborIds[i]);
                        break;
                    }
                }
            }
        }
        doDiscoveryProcessing = true;
    }
    else if (modeIdx == MODE_CATAN)
    {
        if (!playStarted)
        {
            doDiscoveryProcessing = true;
        }
        else if (currentMillis - previousMillis > 75)
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
                setTileValue(catanState.hasRobber ? 0xFF : catanState.rollValue);
                skipRobber = true;
                if (newPlayer != currentPlayer)
                {
                    currentPlayer = newPlayer;
                    sendSetCurrentPlayerRequest();
                }
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

    if (doDiscoveryProcessing)
    {
        if (state != previousState)
        {
            if (((previousState >> btnDiscover) & 1) && ((state >> btnDiscover) & 1) == 0)
            {
                pollDiscovery = true;
                sendIdRequest();
            }

            previousState = state;
        }
        if (catanState.id != EMPTY && pollDiscovery && currentMillis - previousDiscoveryMillis > 10000)
        {
            if (sendDiscoveryStatsRequest())
            {
                previousDiscoveryMillis = currentMillis;
            }
        }
    }
}

void processMessage(const Message &message)
{
    bool doDiscoveryProcessing = false;

    if (modeIdx == MODE_COMPONENT_TEST)
    {
    }
    else if (modeIdx == MODE_NETWORK_TEST)
    {
        if (!message.getSysCommand() && message.getPayloadSize() > 0)
        {
            NetworkTestMessage *command = (NetworkTestMessage *)message.getBody();
            switch (command->command)
            {
            case START_NODE:
                sendIdRequest();
            }
        }
        else
        {
            doDiscoveryProcessing = true;
        }
    }
    else if (modeIdx == MODE_CATAN)
    {
        if (message.getSysCommand())
        {
            doDiscoveryProcessing = true;
        }
        else
        {
            CatanMessage *command = (CatanMessage *)message.getBody();
            switch (command->command)
            {
            case SET_ROAD:
                setRoadOwner(*(SetRoadRequest *)command, false);
                break;
            case SET_INITIAL_STATE:
                setInitialState(message.getSource(), *(SetInitialStateRequest *)command);
                break;
            case GET_STATE:
                sendStateResponse(message.getSource(), ((GetStateRequest *)command)->placementInfo);
                break;
            case STATE_RESPONSE:
                reconcileStateResponse(*(StateResponse *)command);
                break;
            case SET_PLAYER:
                setCurrentPlayer(*(SetCurrentPlayerRequest *)command);
                break;
            case CLEAR_ROBBER:
                if (((ClearRobberRequest *)command)->robberId != catanState.id)
                {
                    catanState.hasRobber = false;
                    setTileValue(catanState.rollValue);
                }
            default:
                break;
            }
        }
    }

    if (doDiscoveryProcessing)
    {
        if (message.getSysCommand() == ROUTER_RESPONSE_ID)
        {
            handleIdResponse(message);
            if (modeIdx == MODE_NETWORK_TEST)
            {
                if (pollDiscovery)
                {
                    sendDiscoveryRequest();
                }
                else
                {
                    sendNeighborRequest(catanState.id, true);
                }
            }
            else if (modeIdx == MODE_CATAN)
            {
                pollDiscovery = sendDiscoveryRequest();
            }
        }
        else if (message.getSysCommand() == ROUTER_RESPONSE_DISCOVERY_STATUS)
        {
            handleDiscoveryStatsResponse(message);
        }
        else if (message.getSysCommand() == ROUTER_ADD_NODE)
        {
            if (modeIdx == MODE_NETWORK_TEST)
            {
                handleNodeResponseNetworkTest(message);
            }
            else if (modeIdx == MODE_CATAN)
            {
                handleNodeResponseCatan(message);
            }
        }
    }
}

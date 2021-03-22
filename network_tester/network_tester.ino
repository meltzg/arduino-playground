#include <AltSoftSerial.h>
#include <CommonMessaging.h>
#include <Components.h>

#define SEGMENT_LATCH A0
#define SEGMENT_CLOCK A1
#define SEGMENT_DATA A2
#define SEGMENT_LEFT A3
#define SEGMENT_RIGHT A4

#define LED_ARRAY 7
#define BRIGHTNESS 50

#define BTN_LOAD 10
#define BTN_CLK_ENABLE 11
#define BTN_DATA 12
#define BTN_CLOCK 13

#define SEED_PIN A5

#define NUM_LEDS 11
#define NUM_BUTTONS 10

#define BTN_ID 9
#define BTN_NEIGHBORS 1
#define BTN_DISCOVER 10

const byte NEIGHBOR_LED_POS[] = {0, 3, 6, 7, 8, 9};

SegmentDisplay disp(
    SEGMENT_LATCH,
    SEGMENT_CLOCK,
    SEGMENT_DATA,
    SEGMENT_LEFT,
    SEGMENT_RIGHT,
    300);

LEDStatusDisplay leds(LED_ARRAY, NUM_LEDS);

ButtonArray16 btns(
    BTN_LOAD,
    BTN_CLK_ENABLE,
    BTN_DATA,
    BTN_CLOCK);

AltSoftSerial netPort;

__int24 ledColors[NUM_LEDS] = {BLACK};
uint16_t previousState = 0;
NodeId_t myId = 0;

char displayMessage[100] = { 0 };

void setup()
{
    Serial.begin(9600);
    netPort.begin(9600);

    Serial.println("Start");

    disp.setChars("Start ");
    leds.setState(ledColors, BRIGHTNESS);
}

void loop()
{
    unsigned long currentMillis = millis();
    disp.render(currentMillis);
    btns.render(currentMillis);
    leds.render(currentMillis);

    uint16_t state = btns.getState();
    if (state != previousState)
    {
        if (((previousState >> BTN_ID) & 1) && ((state >> BTN_ID) & 1) == 0)
        {
            handleIdRequest();
        }
        else if (((previousState >> BTN_NEIGHBORS) & 1) && ((state >> BTN_NEIGHBORS) & 1) == 0)
        {
            handleNeighborRequest();
        }
        else if (((previousState >> BTN_DISCOVER) & 1) && ((state >> BTN_DISCOVER) & 1) == 0)
        {
            handleDiscoveryRequest();
        }
        previousState = state;
    }
}

void handleIdRequest()
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

void handleNeighborRequest()
{
    int maxRetries = 1000;
    Message neighborRequest;
    neighborRequest.source = myId;
    neighborRequest.dest = myId;
    neighborRequest.payloadSize = 0;
    neighborRequest.sysCommand = ROUTER_GET_NEIGHBORS;
    neighborRequest.body = NULL;

    if (ackWait(&netPort, maxRetries))
    {
        writeMessage(&netPort, neighborRequest);
        while (!hasIncoming(&netPort))
        {
        }
        Message response = readMessage(&netPort);
        sprintf(
            displayMessage,
            "Neighbors [%04X, %04X, %04X, %04X, %04X, %04X] ",
            ((NodeId_t *)(response.body))[1],
            ((NodeId_t *)(response.body))[2],
            ((NodeId_t *)(response.body))[3],
            ((NodeId_t *)(response.body))[4],
            ((NodeId_t *)(response.body))[5],
            ((NodeId_t *)(response.body))[6]);

        disp.setChars(displayMessage);
        Serial.println(displayMessage);
        for (int i = 0; i < 6; i++)
        {
            if (((NodeId_t *)(response.body))[i + 1] == EMPTY)
            {
                ledColors[NEIGHBOR_LED_POS[i]] = RED;
            }
            else
            {
                ledColors[NEIGHBOR_LED_POS[i]] = GREEN;
            }
        }
        leds.setState(ledColors, BRIGHTNESS);
    }
    else
    {
        disp.setChars("Failure ");
        Serial.println("Failure");
        return;
    }
}

void handleDiscoveryRequest() {}

#include "modes.h"

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

// Mode definitions
#define NUM_MODES 3
#define MODE_COMPONENT_TEST 0
#define MODE_NETWORK_TEST 1
#define MODE_CATAN 2

#define BTN_CENTER 8

// Component setup
SegmentDisplay disp(
    SEGMENT_LATCH,
    SEGMENT_CLOCK,
    SEGMENT_DATA,
    SEGMENT_LEFT,
    SEGMENT_RIGHT,
    500,
    false);

LEDStatusDisplay leds(LED_ARRAY, NUM_LEDS);

ButtonArray16 btns(
    BTN_LOAD,
    BTN_CLK_ENABLE,
    BTN_DATA,
    BTN_CLOCK);

SoftwareSerial netPort(8, 9);

// Modes
ComponentTestMode componentTest(disp, leds, btns, netPort);
NetworkTestMode networkTest(disp, leds, btns, netPort);
CatanMode catan(disp, leds, btns, netPort);

// State information
Mode *mode = NULL;
byte modeIdx = MODE_COMPONENT_TEST;
unsigned long previousMillis = 0;
uint16_t previousState = 0;

void setup()
{
    Serial.begin(9600);
    netPort.begin(SOFT_BAUD);

    __int24 ledColors[NUM_LEDS] = {BLACK};
    leds.setState(ledColors);
    selectMode(modeIdx);
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
        modeIdx++;
        modeIdx %= NUM_MODES;

        selectMode(modeIdx);
    }

    if (hasIncoming(&netPort))
    {
        Message message = readMessage(&netPort);
        mode->processMessage(message);
        delete[] message.body;
        message.body = NULL;
    }
    mode->processState(currentMillis, btnState);

    previousState = btnState;
}

void selectMode(int modeIdx)
{
    switch (modeIdx)
    {
    case MODE_COMPONENT_TEST:
        mode = &componentTest;
        break;
    case MODE_NETWORK_TEST:
        mode = &networkTest;
        break;
    case MODE_CATAN:
        mode = &catan;
    default:
        break;
    }

    mode->init();
}

#include <SoftwareSerial.h>
#include <CommonMessaging.h>
#include <Components.h>

// Hardware config
#define SEGMENT_LATCH A0
#define SEGMENT_CLOCK A1
#define SEGMENT_DATA A2
#define SEGMENT_LEFT A3
#define SEGMENT_RIGHT A4

#define LED_ARRAY 7
#define NUM_LEDS 11
#define BRIGHTNESS 50

#define NUM_BUTTONS 10
#define BTN_LOAD 10
#define BTN_CLK_ENABLE 11
#define BTN_DATA 12
#define BTN_CLOCK 13

// Mode definitions
#define NUM_MODES 2
#define MODE_COMPONENT_TEST 0
#define MODE_NETWORK_TEST 1

#define BTN_CENTER 8

// Mode Specific Defines
// Netowrk test
#define BTN_ID 9
#define BTN_NEIGHBORS 1
#define BTN_DISCOVER 10

const byte EDGE_LED_POS[] = {0, 3, 6, 7, 8, 9};
const byte EDGE_BTN_POS[] = {0, 2, 4, 5, 6, 7};

// State information
byte mode = MODE_COMPONENT_TEST;
unsigned long previousMillis = 0;
uint16_t previousState = 0;
NodeId_t myId = EMPTY;
NodeId_t neighborIds[6];
char displayMessage[100] = {0};

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

void setup()
{
  Serial.begin(9600);
  netPort.begin(SOFT_BAUD);

  __int24 ledColors[NUM_LEDS] = {BLACK};
  leds.setState(ledColors, 50);

  for (int i = 0; i < 6; i++)
  {
    neighborIds[i] = EMPTY;
  }
}

void loop()
{
  bool modeChange = false;
  unsigned long currentMillis = millis();
  disp.render(currentMillis);
  leds.render(currentMillis);
  btns.render(currentMillis);

  uint16_t btnState = btns.getState();

  if (((previousState >> BTN_CENTER) & 1) && ((btnState >> BTN_CENTER) & 1) == 0)
  {
    mode++;
    mode %= NUM_MODES;
    modeChange = true;
    Serial.println(mode);
  }

  switch (mode)
  {
  case MODE_COMPONENT_TEST:
    if (modeChange)
    {
      disp.setRenderChars(false);
    }
    processComponentTest(currentMillis, btnState);
    break;
  case MODE_NETWORK_TEST:
    if (modeChange)
    {
      disp.setRenderChars(true);
      __int24 ledColors[NUM_LEDS];
      for (int i = 0; i < NUM_LEDS; i++)
      {
        ledColors[i] = BLACK;
      }
      leds.setState(ledColors, BRIGHTNESS);
      disp.setChars("Start ");
    }
    processNetworkTest(currentMillis, btnState);
    break;
  default:
    break;
  }

  previousState = btnState;
}

void processComponentTest(long currentMillis, uint16_t btnState)
{
  static int colorOffset = 0;

  if (currentMillis - previousMillis > 500)
  {
    __int24 ledColors[NUM_LEDS] = {BLACK};
    for (int i = 0; i < NUM_LEDS; i++)
    {
      if (((1 << i) & btnState) > 0)
      {
        continue;
      }
      ledColors[i] = RAINBOW[(i + colorOffset) % 6];
    }
    colorOffset++;
    leds.setState(ledColors, BRIGHTNESS);
    disp.registerWrite(~(1 << (colorOffset % 16)));

    previousMillis = currentMillis;
  }
}

void processNetworkTest(long currentMillis, uint16_t btnState)
{
  if (hasIncoming(&netPort))
  {
    Serial.println("recieving message");
    Message message = readMessage(&netPort);
    processMessage(&netPort, message);
    delete[] message.body;
    message.body = NULL;
  }

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
  }
}

void processMessage(Stream *srcPort, const Message &message)
{
  if (message.sysCommand & ROUTER_ADD_NODE)
  {
    handleNodeResponse(message);
  }
}

void handleNodeResponse(Message message)
{
  __int24 ledColors[NUM_LEDS] = {BLACK};
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
  leds.setState(ledColors, BRIGHTNESS);
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

void handleNeighborRequest(NodeId_t destination)
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

void handleDiscoveryRequest() {}

#ifndef _MODES_H_
#define _MODES_H_

#include <SoftwareSerial.h>
#include <CommonMessaging.h>
#include <Components.h>

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

// Mode Specific Defines
// Netowrk test
#define BTN_ID 9
#define BTN_NEIGHBORS 1
#define BTN_DISCOVER 10

const byte EDGE_LED_POS[] = {0, 3, 6, 7, 8, 9};
const byte EDGE_BTN_POS[] = {0, 2, 4, 5, 6, 7};

class Mode
{
protected:
    const SegmentDisplay &disp;
    const LEDStatusDisplay &leds;
    const ButtonArray16 &btns;
    const SoftwareSerial &netPort;

    unsigned long previousMillis = 0;
    uint16_t previousState = 0;

public:
    Mode(const SegmentDisplay &disp, const LEDStatusDisplay &leds, const ButtonArray16 &btns, const SoftwareSerial &netPort) : disp(disp), leds(leds), btns(btns), netPort(netPort) { init(); }
    virtual void init() {}
    virtual void process(unsigned long currentMillis) {};
};

class ComponentTestMode : public Mode
{
public:
    using Mode::Mode;

    virtual void init();
    virtual void process(unsigned long currentMillis);
};

class NetworkTestMode : public Mode
{
public:
    using Mode::Mode;

    virtual void init();
    virtual void process(unsigned long currentMillis);

private:
    NodeId_t myId = EMPTY;
    NodeId_t neighborIds[6];
    char displayMessage[100] = {0};

    void processMessage(Stream *srcPort, const Message &message);
    void handleNodeResponse(const Message &message);
    void handleIdRequest();
    void handleNeighborRequest(NodeId_t destination);
    void handleDiscoveryRequest() {}
};

#endif // _MODES_H_

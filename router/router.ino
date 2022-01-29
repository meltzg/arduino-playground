#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include "CommonMessaging.h"
#include "DataStructures.h"
#include "PathFinder.h"

#define PRINT_BUF_SIZE 100

#define NEIGHBOR_RETRIES 500
#define DISCOVERER_UPDATE_FREQUENCY 10000

/*
   Each node has a SoftwareSerial connection to its neighbor and another to the
   1 /\ 2
   0 | | 3
   5 \/ 4

   A 7th SoftwareSerial represents the actual destination on the node

   System Commands
   ---------------
   Get ID: 0x01
     Retrieve the ID of the node. During newtork discovery, this can be used
     to determine the ID of one's neightbors

   Get Neighbors: 0x02
     Retrieve the neighbors of the Source address
*/

#if defined(__AVR_ATmega328P__)
SoftwareSerial PORT_0(2, 3);
SoftwareSerial PORT_1(4, 5);
SoftwareSerial PORT_2(6, 7);
SoftwareSerial PORT_3(8, 9);
SoftwareSerial PORT_4(10, 11);
SoftwareSerial PORT_5(12, 13);
SoftwareSerial PORT_A(14, 15);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
SoftwareSerial PORT_0(10, 11);
SoftwareSerial PORT_1(12, 13);
SoftwareSerial PORT_2(50, 51);
SoftwareSerial PORT_3(52, 53);
SoftwareSerial PORT_4(62, 63);
SoftwareSerial PORT_5(65, 66);
SoftwareSerial PORT_A(67, 68);
#endif

SoftwareSerial *NEIGHBORS[6] = {&PORT_0, &PORT_1, &PORT_2, &PORT_3, &PORT_4, &PORT_5};
SoftwareSerial *ALL_PORTS[7] = {&PORT_0, &PORT_1, &PORT_2, &PORT_3, &PORT_4, &PORT_5, &PORT_A};
NodeId_t neighborIds[6] = {NULL};

NodeId_t NODE_ID;

PathFinder pathfinder(6);

Set<NodeId_t> pendingNeighborRequests;
unsigned char pendingIdRequests = 0;
bool pendingDiscovery = false;
unsigned long previousDiscoveryStatsUpdate = 0;
NodeId_t discoverId = EMPTY;

void setup()
{
    delay(2000);
    NODE_ID = getNodeId();
    Serial.begin(9600);
    Wire.begin();
    startPorts();

    Serial.print(F("starting "));
    Serial.println(NODE_ID, HEX);
    logDiscoveryStats();
    EEPROM.get(0, neighborIds);
    for (int i = 0; i < 6; i++)
    {

        Serial.print(F("Neighbor "));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.println(neighborIds[i], HEX);
    }
}

void loop()
{
    // Loop through ports and process messages
    if (Serial.available() > 0)
    {
        Serial.println(F("Serial available"));
        Message message = readMessage(&Serial);
        message.setSource(PORT_H);
        processMessage(&Serial, message);
        message.free();
    }

    for (int i = 0; i < 6; i++)
    {
        SoftwareSerial *port = NEIGHBORS[i];
        port->listen();
        Message message = readMessage(port);
        if (message.isValid())
        {
            Serial.println(F("recieving message"));
            Serial.write((char *)message.getBody(), message.getPayloadSize());
            Serial.println();
            processMessage(port, message);
            message.free();
        }
    }

    PORT_A.listen();
    Message message = readMessage(&PORT_A);
    if (message.isValid())
    {
        Serial.println(F("from user"));
        message.setSource(NODE_ID);
        Serial.write((char *)message.getBody(), message.getPayloadSize());
        Serial.println();
        processMessage(&PORT_A, message);
        message.free();
    }
}

void startPorts()
{
    Serial.println(F("Starting portd"));
    PORT_0.begin(SOFT_BAUD);
    PORT_1.begin(SOFT_BAUD);
    PORT_2.begin(SOFT_BAUD);
    PORT_3.begin(SOFT_BAUD);
    PORT_4.begin(SOFT_BAUD);
    PORT_5.begin(SOFT_BAUD);
    PORT_A.begin(SOFT_BAUD);
}

void stopPorts()
{
    Serial.println(F("Stopping portd"));
    PORT_0.end();
    PORT_1.end();
    PORT_2.end();
    PORT_3.end();
    PORT_4.end();
    PORT_5.end();
    PORT_A.end();
}

void logDiscoveryStats()
{
    DiscoveryStats stats = pathfinder.getDiscoveryStats();
    char buf[50];
    sprintf(buf, "D: %d, N: %d, E: %d    ", stats.discoveryDone, stats.numNodes, stats.numEdges);
    Serial.println(buf);
    if (discoverId != EMPTY && (stats.discoveryDone || (millis() - previousDiscoveryStatsUpdate > DISCOVERER_UPDATE_FREQUENCY)))
    {
        Serial.print(F("Send status to discoverId "));
        Serial.println(discoverId, HEX);
        sendDiscoveryStats(discoverId);
        previousDiscoveryStatsUpdate = millis();
        if (stats.discoveryDone)
        {
            discoverId = EMPTY;
        }
    }
}

void processMessage(Stream *srcPort, const Message &message)
{
    char buf[PRINT_BUF_SIZE];
    if (message.getSysCommand() == ROUTER_GET_ID)
    {
        // Get ID is usually only used when the sender doesn't know the ID of the node, so just send it back the same srcPort
        sprintf(buf, "Node ID requested by %hx", message.getSource());
        Serial.println(buf);
        Message response(
            NODE_ID,
            message.getSource(),
            sizeof(NodeId_t),
            message.getSysOption(),
            ROUTER_RESPONSE_ID,
            (byte *)(&NODE_ID));
        writeMessage(srcPort, response);
        return;
    }
    if (message.getSysOption() & ROUTER_BROADCAST)
    {
        Serial.println(F("Broadcasting message"));
        pathfinder.resetIterator(NODE_ID);
        for (NodeId_t distribId = pathfinder.getIteratorNext(); distribId != EMPTY; distribId = pathfinder.getIteratorNext())
        {
            Serial.println(distribId, HEX);
            message.setDest(distribId);
            message.setSysOption(message.getSysOption() & ~ROUTER_BROADCAST);
            routeMessage(message);
        }
    }
    if (message.getDest() != NODE_ID)
    {
        if (!(message.getSysOption() & ROUTER_USE_CACHE) || !(message.getSysCommand() & ROUTER_GET_NEIGHBORS))
        {
            routeMessage(message);
            return;
        }
    }
    if (message.getSysCommand() == ROUTER_RESPONSE_ID)
    {
        pendingIdRequests--;
        for (int i = 0; i < 6; i++)
        {
            if (NEIGHBORS[i] == srcPort)
            {
                neighborIds[i] = *((NodeId_t *)message.getBody());
                Serial.print(F("Neighbor "));
                Serial.print(i);
                Serial.print(F(": "));
                Serial.println(neighborIds[i], HEX);
            }
        }
        updateNeighborIds(message.getSysOption() & ROUTER_SYS_COMMAND);
    }
    if (message.getSysOption() & ROUTER_CLEAR_TOPOLOGY)
    {
        Serial.println(F("Clear topology"));
        pathfinder.clearTopology();
    }
    if (message.getSysCommand() == ROUTER_GET_NEIGHBORS)
    {
        sprintf(buf, "Node Neighbors requested by %hx", message.getSource());
        Serial.println(buf);
        int numNodes = 0;
        NodeId_t *nodeMessage = NULL;

        if (message.getDest() == NODE_ID)
        {
            Set<NodeId_t> adj;
            pathfinder.getAdjacent(message.getDest(), adj);
            for (int i = 0; i < 6; i++)
            {
                if (neighborIds[i] != EMPTY)
                {
                    numNodes++;
                }
            }
            if (!(message.getSysOption() & ROUTER_USE_CACHE) || numNodes < adj.count)
            {
                pendingNeighborRequests.pushBack(message.getSource());
                resetNeighbors(message.getSysOption() & ROUTER_SYS_COMMAND);
                return;
            }
            numNodes = 7;
            nodeMessage = new NodeId_t[numNodes];
            nodeMessage[0] = NODE_ID;
            for (int i = 0; i < 6; i++)
            {
                nodeMessage[i + 1] = neighborIds[i];
            }
        }
        else
        {
            Set<NodeId_t> adj;
            pathfinder.getAdjacent(message.getDest(), adj);
            ListIterator<NodeId_t> adjIter(adj);

            numNodes = 1 + adj.count;
            nodeMessage = new NodeId_t[numNodes];
            nodeMessage[0] = message.getDest();
            for (int i = 1; adjIter.hasNext(); i++)
            {
                nodeMessage[i] = adjIter.next();
            }
        }

        Message response(
            NODE_ID,
            message.getSource(),
            sizeof(NodeId_t) * numNodes,
            message.getSysOption() & ROUTER_SYS_COMMAND,
            ROUTER_ADD_NODE,
            (byte *)nodeMessage);
        routeMessage(response);
        delete[] nodeMessage;
        return;
    }
    if (message.getSysCommand() == ROUTER_ADD_NODE)
    {
        sprintf(buf, "Adding node to topology", message.getSource());
        Serial.println(buf);
        NodeId_t *nodeIds = (NodeId_t *)message.getBody();
        updateNeighbors(nodeIds[0], nodeIds + 1, (message.getPayloadSize() / sizeof(NodeId_t)) - 1);
    }
    if (message.getSysCommand() == ROUTER_START_DISCOVERY)
    {
        Serial.println(F("Start Discovery"));
        startDiscovery(message.getSource(), message.getSysOption() & ROUTER_ENABLE_DISCOVERY_UPDATES);
        return;
    }
    if (message.getSysCommand() == ROUTER_GET_DISCOVERY_STATUS)
    {
        Serial.println(F("Get discovery stats"));
        sendDiscoveryStats(message.getSource());
        return;
    }
    if (message.getSysOption() & ROUTER_SYS_COMMAND)
    {
        // return without sending message to actor
        Serial.println(F("Message not for actor"));
        return;
    }
    routeMessage(message);
}

void routeMessage(const Message &message)
{
    char buf[PRINT_BUF_SIZE];
    sprintf(buf, "Routing message from %hx to %hx via %hx size %hu", message.getSource(), message.getDest(), NODE_ID, message.getPayloadSize());
    Serial.println(buf);
    if (message.getDest() == PORT_H)
    {
        writeMessage(&Serial, message);
        return;
    }

    NodeId_t nextStep = EMPTY;
    for (int i = 0; i < 6; i++)
    {
        if (neighborIds[i] == message.getDest())
        {
            nextStep = message.getDest();
            break;
        }
    }

    if (nextStep == EMPTY)
    {
        if (message.getDest() == NODE_ID)
        {
            nextStep = NODE_ID;
        }
        else
        {
            nextStep = pathfinder.getNextStep(NODE_ID, message.getDest());
        }
    }

    if (nextStep == EMPTY)
    {
        Serial.println(F("path not found"));
        return;
    }

    Serial.print(F("Path found "));
    Serial.println(nextStep != EMPTY);

    if (nextStep == NODE_ID)
    {
        Serial.print(F("Route to actor"));
        PORT_A.listen();
        writeMessage(&PORT_A, message);
    }
    else
    {
        for (int i = 0; i < 6; i++)
        {
            if (neighborIds[i] == nextStep)
            {
                Serial.print(F("route through "));
                Serial.println(i);
                NEIGHBORS[i]->listen();
                writeMessage(NEIGHBORS[i], message);
                break;
            }
        }
    }
    Serial.println(F("route complete"));
}

void resetNeighbors(bool isSysCommand)
{
    Serial.println(F("Reset Edges"));
    pendingIdRequests = 0;
    Message idRequest(
        NODE_ID,
        EMPTY,
        0,
        ROUTER_SYS_COMMAND,
        ROUTER_GET_ID,
        NULL);
    for (int i = 0; i < 6; i++)
    {
        NEIGHBORS[i]->listen();
        neighborIds[i] = EMPTY;
        if (writeMessage(NEIGHBORS[i], idRequest, NEIGHBOR_RETRIES))
        {
            pendingIdRequests++;
        }
    }
    updateNeighborIds(isSysCommand);
}

void updateNeighborIds(bool isSysCommand)
{
    EEPROM.put(0, neighborIds);
    Serial.print(F("pending "));
    Serial.println(pendingIdRequests);
    if (pendingIdRequests == 0)
    {
        int numNodes = 7;
        NodeId_t nodeMessage[numNodes];
        nodeMessage[0] = NODE_ID;
        for (int i = 0; i < 6; i++)
        {
            nodeMessage[i + 1] = neighborIds[i];
        }
        Message response(
            NODE_ID,
            NODE_ID,
            sizeof(NodeId_t) * numNodes,
            isSysCommand ? ROUTER_SYS_COMMAND : 0,
            ROUTER_ADD_NODE,
            (byte *)nodeMessage);

        while (!pendingNeighborRequests.isEmpty())
        {
            response.setDest(pendingNeighborRequests.popFront());
            routeMessage(response);
        }
        if (pendingDiscovery)
        {
            pendingDiscovery = false;
            updateNeighbors(NODE_ID, neighborIds, 6);
        }
    }
}

void updateNeighbors(NodeId_t src, NodeId_t *neighbors, int numNeighbors)
{
    if (numNeighbors <= 0)
    {
        return;
    }

    bool doDistribute = !pathfinder.getDiscoveryStats().discoveryDone;

    char buf[PRINT_BUF_SIZE];

    if (!doDistribute)
    {
        pathfinder.addNode(src, neighbors, numNeighbors);
        logDiscoveryStats();
        return;
    }

    Set<NodeId_t> uninitialized;

    NodeId_t nodeForward[numNeighbors + 1];
    nodeForward[0] = src;
    for (int i = 0; i < numNeighbors; i++)
    {
        nodeForward[i + 1] = neighbors[i];
        if (neighbors[i] != EMPTY && pathfinder.getNextStep(NODE_ID, neighbors[i]) == EMPTY && neighbors[i] != NODE_ID)
        {
            Serial.print(F("B must initialize "));
            Serial.println(neighbors[i], HEX);
            uninitialized.pushBack(neighbors[i]);
        }
    }

    Message message(
        NODE_ID,
        EMPTY,
        (numNeighbors + 1) * sizeof(NodeId_t),
        ROUTER_SYS_COMMAND,
        ROUTER_ADD_NODE,
        (byte *)nodeForward);

    // Add edges to pathfinder
    pathfinder.addNode(src, neighbors, numNeighbors);
    pathfinder.resetIterator(NODE_ID);
    for (NodeId_t distribId = pathfinder.getIteratorNext(); distribId != EMPTY; distribId = pathfinder.getIteratorNext())
    {
        if (distribId == NODE_ID || distribId == src || uninitialized.contains(distribId))
        {
            Serial.print(F("skip update"));
            Serial.println(distribId, HEX);
            continue;
        }
        Serial.println(F("Send new node to existing"));
        message.setDest(distribId);
        routeMessage(message);
    }

    Serial.println(F("Send entire graph to new nodes"));
    pathfinder.resetIterator(NODE_ID);
    if (!uninitialized.isEmpty())
    {
        for (NodeId_t distribId = pathfinder.getIteratorNext(); distribId != EMPTY; distribId = pathfinder.getIteratorNext())
        {
            sprintf(buf, "creating add node message for %hx: ", distribId);
            Serial.print(buf);
            Set<NodeId_t> adj;
            pathfinder.getAdjacent(distribId, adj);
            ListIterator<NodeId_t> adjIter(adj);

            int numAdj = 1 + adj.count;
            NodeId_t nodeMessage[numAdj] = {0};
            nodeMessage[0] = distribId;
            for (int i = 1; adjIter.hasNext(); i++)
            {
                nodeMessage[i] = adjIter.next();
                sprintf(buf, "%hx, ", nodeMessage[i]);
                Serial.print(buf);
            }
            Serial.println();

            Message message(
                NODE_ID,
                src,
                numAdj * sizeof(NodeId_t),
                ROUTER_SYS_COMMAND,
                ROUTER_ADD_NODE,
                (byte *)nodeMessage);

            if (distribId == NODE_ID)
            {
                // clear destination's topology to ensure it always has the most correct graph
                // Needed in the case of rediscovery
                Serial.println(F("Destination will clear its topology"));
                message.setSysOption(message.getSysOption() | ROUTER_CLEAR_TOPOLOGY);
            }
            for (ListIterator<NodeId_t> iter(uninitialized); iter.hasNext();)
            {
                message.setDest(iter.next());
                routeMessage(message);
            }
        }
    }

    NodeId_t next = pathfinder.getNextNeighborRequest();
    if (next != EMPTY)
    {
        Serial.print(F("Sending get neighbor request: "));
        Serial.println(next, HEX);

        Message neighborRequest(
            NODE_ID,
            next,
            0,
            ROUTER_SYS_COMMAND,
            ROUTER_GET_NEIGHBORS,
            NULL);

        routeMessage(neighborRequest);
    }
    else
    {
        Serial.println(F("No next neighbor"));
    }

    logDiscoveryStats();
}

void startDiscovery(NodeId_t src, bool enableDiscoveryUpdates)
{
    pathfinder.startDiscovery();

    resetNeighbors(true);
    pendingDiscovery = true;
    if (enableDiscoveryUpdates)
    {
        discoverId = src;
    }
    else
    {
        discoverId = EMPTY;
    }
}

void sendDiscoveryStats(NodeId_t dst)
{
    DiscoveryStats stats = pathfinder.getDiscoveryStats();
    bool discoveryDone = stats.discoveryDone;
    size_t numNodes = stats.numNodes;
    size_t numEdges = stats.numEdges;

    Serial.print(F("sizeof discover stat "));
    Serial.println(sizeof(stats));
    Message response(
        NODE_ID,
        dst,
        sizeof(DiscoveryStats),
        0,
        ROUTER_RESPONSE_DISCOVERY_STATUS,
        (char *)(&stats));
    routeMessage(response);
}

int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++)
    {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

NodeId_t getNodeId()
{
    NodeId_t nodeId = 0;

    int datelen = strlen(__DATE__);
    int timelen = strlen(__TIME__);
    char date[datelen + 1] = {0};
    char time_[timelen + 1] = {0};
    strncpy(date, __DATE__, datelen);
    strncpy(time_, __TIME__, timelen);
    char *token;

    // add date to node ID
    int year, month, day;
    token = strtok(date, " :");
    day = atoi(strtok(NULL, " :"));
    year = atoi(strtok(NULL, " :"));

    nodeId += year;

    nodeId = nodeId << 4;
    if (strcicmp(token, "jan") == 0)
    {
        nodeId += 1;
    }
    else if (strcicmp(token, "feb") == 0)
    {
        nodeId += 2;
    }
    else if (strcicmp(token, "mar") == 0)
    {
        nodeId += 3;
    }
    else if (strcicmp(token, "apr") == 0)
    {
        nodeId += 4;
    }
    else if (strcicmp(token, "may") == 0)
    {
        nodeId += 5;
    }
    else if (strcicmp(token, "jun") == 0)
    {
        nodeId += 6;
    }
    else if (strcicmp(token, "jul") == 0)
    {
        nodeId += 7;
    }
    else if (strcicmp(token, "aug") == 0)
    {
        nodeId += 8;
    }
    else if (strcicmp(token, "sep") == 0)
    {
        nodeId += 9;
    }
    else if (strcicmp(token, "oct") == 0)
    {
        nodeId += 10;
    }
    else if (strcicmp(token, "nov") == 0)
    {
        nodeId += 11;
    }
    else if (strcicmp(token, "dec") == 0)
    {
        nodeId += 12;
    }

    nodeId = nodeId << 5;
    nodeId += day;

    // add time to node ID
    token = strtok(time_, " :");
    for (int i = 0; i < 3 && token != NULL; i++)
    {
        nodeId = nodeId << 6;
        nodeId += atoi(token);
        token = strtok(NULL, " :");
    }

    return nodeId;
}

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif // __arm__

int freeMemory()
{
    char top;
#ifdef __arm__
    return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
#else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif // __arm__
}

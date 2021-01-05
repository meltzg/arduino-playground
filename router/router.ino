#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include "CommonMessaging.h"
#include "DataStructures.h"
#include "PathFinder.h"

#define PRINT_BUF_SIZE 100

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
NodeId_t neighborIds[6] = { NULL };

NodeId_t NODE_ID;

PathFinder pathfinder(6);

void setup() {
  NODE_ID = getNodeId();
  Serial.begin(9600);
  Wire.begin();
  startPorts();

  Serial.println("starting");
}

void loop() {
  // Loop through ports and process messages
  if (Serial.available() > 0) {
    Message message = readMessage(&Serial);
    message.source = PORT_H;
    processMessage(&Serial, message);
    delete[] message.body;
    message.body = NULL;
  }

  for (int i = 0; i < 7; i++) {
    SoftwareSerial *port = ALL_PORTS[i];
    port->listen();
    if (hasIncoming(port)) {
      Serial.println("recieving message");
      Message message = readMessage(port);
      Serial.write((char *)message.body, message.payloadSize);
      Serial.println();
      processMessage(port, message);
      delete[] message.body;
      message.body = NULL;
    }
  }
}

void startPorts() {
  Serial.println("Starting portd");
  PORT_0.begin(9600);
  PORT_1.begin(9600);
  PORT_2.begin(9600);
  PORT_3.begin(9600);
  PORT_4.begin(9600);
  PORT_5.begin(9600);
  PORT_A.begin(9600);
}

void stopPorts() {
  Serial.println("Stopping portd");
  PORT_0.end();
  PORT_1.end();
  PORT_2.end();
  PORT_3.end();
  PORT_4.end();
  PORT_5.end();
  PORT_A.end();
}

void processMessage(Stream * srcPort, const Message &message) {
  char buf[PRINT_BUF_SIZE];
  if (message.sysCommand & ROUTER_GET_ID) {
    // Get ID is usually only used when the sender doesn't know the ID of the node, so just send it back the same srcPort
    sprintf(buf, "Node ID requested by %hx", message.source);
    Serial.println(buf);
    srcPort->write((char *) &NODE_ID, sizeof(NODE_ID));
    return;
  }
  if (message.dest != NODE_ID) {
    routeMessage(message);
    return;
  }
  if (message.sysCommand & ROUTER_CLEAR_TOPOLOGY) {
    Serial.println("Clear topology");
    pathfinder.clearTopology();
  }
  if (message.sysCommand & ROUTER_GET_NEIGHBORS) {
    sprintf(buf, "Node Neighbors requested by %hx", message.source);
    Serial.println(buf);
    resetNeighbors();
    NodeId_t nodeMessage[7] = { 0 };
    nodeMessage[0] = NODE_ID;
    for (int i = 0; i < 6; i++) {
      nodeMessage[i + 1] = neighborIds[i];
    }
    Message response;
    response.source = NODE_ID;
    response.dest = message.source;
    response.payloadSize = sizeof(nodeMessage);
    response.sysCommand = ROUTER_ADD_NODE;
    response.body = (byte *) nodeMessage;
    routeMessage(response);
  }
  if (message.sysCommand & ROUTER_ADD_NODE) {
    sprintf(buf, "Adding node to topology", message.source);
    Serial.println(buf);
    NodeId_t *nodeIds = (NodeId_t *) message.body;
    updateNeighbors(nodeIds[0], nodeIds + 1, (message.payloadSize / sizeof(NodeId_t)) - 1);
  }
  if (message.sysCommand & ROUTER_START_DISCOVERY) {
    Serial.println("Start Discovery");
    startDiscovery();
  }
  if (message.sysCommand & ROUTER_GET_DISCOVERY_STATUS) {
    Serial.println("Get discovery stats");
    byte discoStats[sizeof(bool) + sizeof(size_t) * 2] = { 0 };

    DiscoveryStats stats = pathfinder.getDiscoveryStats();
    bool discoveryDone = stats.discoveryDone;
    size_t numNodes = stats.numNodes;
    size_t numEdges = stats.numEdges;

    memcpy(discoStats, &discoveryDone, sizeof(bool));
    memcpy(discoStats + sizeof(bool), &numNodes, sizeof(size_t));
    memcpy(discoStats + sizeof(bool) + sizeof(size_t), &numEdges, sizeof(size_t));
    Message response;
    response.source = NODE_ID;
    response.dest = message.source;
    response.payloadSize = sizeof(discoStats);
    response.sysCommand = 0;
    response.body = discoStats;
    routeMessage(response);
  }
}

void routeMessage(const Message &message) {
  char buf[PRINT_BUF_SIZE];
  sprintf(buf, "Routing message from %hx to %hx via %hx size %hu", message.source, message.dest, NODE_ID, message.payloadSize);
  Serial.println(buf);
  if (message.dest == PORT_H) {
    writeMessage(&Serial, message);
    return;
  }
  NodeId_t nextStep = pathfinder.getNextStep(NODE_ID, message.dest);

  if (nextStep == EMPTY) {
    return;
  }

  Serial.print("Path found ");
  Serial.println(nextStep != EMPTY);

  for (int i = 0; i < 6; i++) {
    if (neighborIds[i] == nextStep) {
      NEIGHBORS[i]->listen();
      ackWait(NEIGHBORS[i]);
      writeMessage(NEIGHBORS[i], message);
      break;
    }
  }
}

void resetNeighbors() {
  Serial.println("Reset Edges");
  int maxRetries = 2 * (LISTEN_WAIT * 8 / PING_DELAY);
  Message idRequest;
  idRequest.source = NODE_ID;
  idRequest.dest = EMPTY;
  idRequest.payloadSize = 0;
  idRequest.sysCommand = ROUTER_GET_ID;
  idRequest.body = NULL;
  for (int i = 0; i < 6; i++) {
    NEIGHBORS[i]->listen();
    if (ackWait(NEIGHBORS[i], maxRetries)) {
      writeMessage(NEIGHBORS[i], idRequest);
      NodeId_t neighbor = EMPTY;
      NEIGHBORS[i]->readBytes((byte *) &neighbor, sizeof(NodeId_t));
      neighborIds[i] = neighbor;
    } else {
      neighborIds[i] = EMPTY;
    }
    Serial.print(neighborIds[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
}

void updateNeighbors(NodeId_t src, NodeId_t *neighbors, int numNeighbors) {
  if (numNeighbors <= 0) {
    return;
  }

  char buf[PRINT_BUF_SIZE];

  // Add edges to pathfinder
  pathfinder.addNode(src, neighbors, numNeighbors);

  if (pathfinder.getDiscoveryStats().discoveryDone) {
    return;
  }

  Serial.println("Continuing discover");

  // update all nodes with the new one
  pathfinder.resetIterator(NODE_ID);
  Message message;
  message.source = NODE_ID;
  message.payloadSize = numNeighbors * sizeof(NodeId_t);
  message.sysCommand = ROUTER_ADD_NODE;
  message.body = (byte *) neighbors;
  for (NodeId_t distribId = pathfinder.getIteratorNext(); distribId != EMPTY; distribId = pathfinder.getIteratorNext()) {
    if (distribId == NODE_ID || distribId == src) {
      continue;
    }
    message.dest = distribId;
    if (src == NODE_ID) {
      // clear destination's topology to ensure it always has the most correct graph
      // Needed in the case of rediscovery
      Serial.println("Destination will clear its topology");
      message.sysCommand |= ROUTER_CLEAR_TOPOLOGY;
    }
    routeMessage(message);
  }

  if (src != NODE_ID) {
    // then send entire graph to new node
    pathfinder.resetIterator(NODE_ID);
    for (NodeId_t distribId = pathfinder.getIteratorNext(); distribId != EMPTY; distribId = pathfinder.getIteratorNext()) {
      sprintf(buf, "creating add node message for %hx: ", distribId);
      Serial.print(buf);
      Set<NodeId_t> adj;
      pathfinder.getAdjacent(distribId, adj);
      ListIterator<NodeId_t> adjIter(adj);

      int numAdj = 1 + adj.count;
      NodeId_t nodeMessage[numAdj] = { 0 };
      nodeMessage[0] = distribId;
      for (int i = 1; adjIter.hasNext(); i++) {
        nodeMessage[i] = adjIter.next();
        sprintf(buf, "%hx, ", nodeMessage[i]);
        Serial.print(buf);
      }
      Serial.println();

      Message message;
      message.source = NODE_ID;
      message.dest = src;
      message.payloadSize = numAdj * sizeof(NodeId_t);
      message.sysCommand = ROUTER_ADD_NODE;
      message.body = (byte *) nodeMessage;

      if (distribId == NODE_ID) {
        // clear destination's topology to ensure it always has the most correct graph
        // Needed in the case of rediscovery
        Serial.println("Destination will clear its topology");
        message.sysCommand |= ROUTER_CLEAR_TOPOLOGY;
      }
      routeMessage(message);
    }
  }

  NodeId_t next = pathfinder.getNextNeighborRequest();
  if (next != EMPTY) {
    Serial.print("Sending get neighbor request: ");
    Serial.println(next);

    Message neighborRequest;
    neighborRequest.source = NODE_ID;
    neighborRequest.dest = next;
    neighborRequest.payloadSize = 0;
    neighborRequest.sysCommand = ROUTER_GET_NEIGHBORS;
    neighborRequest.body = NULL;
    routeMessage(neighborRequest);
  } else {
    Serial.println("No next neighbor");
  }

  sprintf(buf, "Total Nodes: %d", pathfinder.getDiscoveryStats().numNodes);
  Serial.println(buf);
}

void startDiscovery() {
  pathfinder.startDiscovery();

  resetNeighbors();
  NodeId_t initialNode[7];
  initialNode[0] = NODE_ID;
  for (int i = 0; i < 6; i++) {
    initialNode[i + 1] = neighborIds[i];
  }

  updateNeighbors(NODE_ID, neighborIds, 6);
}

int strcicmp(char const * a, char const * b)
{
  for (;; a++, b++) {
    int d = tolower((unsigned char) * a) - tolower((unsigned char) * b);
    if (d != 0 || !*a)
      return d;
  }
}

NodeId_t getNodeId() {
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
  if (strcicmp(token, "jan") == 0) {
    nodeId += 1;
  } else if (strcicmp(token, "feb") == 0) {
    nodeId += 2;
  } else if (strcicmp(token, "mar") == 0) {
    nodeId += 3;
  } else if (strcicmp(token, "apr") == 0) {
    nodeId += 4;
  } else if (strcicmp(token, "may") == 0) {
    nodeId += 5;
  } else if (strcicmp(token, "jun") == 0) {
    nodeId += 6;
  } else if (strcicmp(token, "jul") == 0) {
    nodeId += 7;
  } else if (strcicmp(token, "aug") == 0) {
    nodeId += 8;
  } else if (strcicmp(token, "sep") == 0) {
    nodeId += 9;
  } else if (strcicmp(token, "oct") == 0) {
    nodeId += 10;
  } else if (strcicmp(token, "nov") == 0) {
    nodeId += 11;
  } else if (strcicmp(token, "dec") == 0) {
    nodeId += 12;
  }

  nodeId = nodeId << 5;
  nodeId += day;

  // add time to node ID
  token = strtok(time_, " :");
  for (int i = 0; i < 3 && token != NULL; i++) {
    nodeId = nodeId << 6;
    nodeId += atoi(token);
    token = strtok(NULL, " :");
  }

  return nodeId;
}


#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

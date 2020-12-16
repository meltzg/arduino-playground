#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include "structures.h"
#include "messaging.h"

// Hardware serial has a special case address
#define PORT_H 0xffff
#define EMPTY 0x0000
// System commands
#define GET_ID 0x01
#define UPDATE_NEIGHBORS 0x02
#define GET_NEIGHBORS 0x04
#define GET_ROUTING 0x08
#define UPDATE_ROUTING 0x10
#define DISCOVER_TOPOLOGY 0x20

#define STATUS_IDX 0
#define STATUS_DURATION 100
#define BRIGHTNESS 0x10
#define INCOMING 0x00FFFF
#define WAITING 0xE2FC2A
#define PROCESSING 0x0000FF
#define SENDING 0xC8991F
#define FAIL 0x00FF00

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

   Update Neighbors: 0x02
     Payload contains the ordered neighbor IDs of the source node

   Get Neighbors: 0x04
     Retrieve the neighbors of the Source address

   Get Routing Table: 0x08
     Return the routing table of the node

   Update Routing Table: 0x10
     The payload contains all known edges in the network to be used to update routing table

   Discover: 0x20
     Discover network topology, create and distribute routing tables
*/

#if defined(__AVR_ATmega328P__)
#define STATUS_LED A2
SoftwareSerial PORT_0(2, 3);
SoftwareSerial PORT_1(4, 5);
SoftwareSerial PORT_2(6, 7);
SoftwareSerial PORT_3(8, 9);
SoftwareSerial PORT_4(10, 11);
SoftwareSerial PORT_5(12, 13);
SoftwareSerial PORT_A(14, 15);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define STATUS_LED A0
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
NodeId_t neighborIds[6] = { EMPTY };

NodeId_t NODE_ID;

LinkedList<NodeId_t> discoveryQueue;
LinkedList<NodeId_t> discoverVisited;
Graph<NodeId_t> topology;
int outstandingNeighborRequests = 0;
bool discoveryDone = false;

Adafruit_NeoPixel pixels(1, STATUS_LED, NEO_GRB + NEO_KHZ800);

void setup() {
  NODE_ID = getNodeId();
  Serial.begin(9600);
  PORT_0.begin(9600);
  PORT_1.begin(9600);
  PORT_2.begin(9600);
  PORT_3.begin(9600);
  PORT_4.begin(9600);
  PORT_5.begin(9600);
  pinMode(STATUS_LED, OUTPUT);

  Serial.println("starting");

  setStatusLed(0xFF0000, 1000);
}

void loop() {
  NodeId_t source, dest;
  MessageSize_t payloadSize;
  SysCommand_t sysCommand;
  byte *body;
  // Loop through ports and process messages
  if (Serial.available() > 0) {
    readMessage(&Serial, source, dest, payloadSize, sysCommand, body);
    source = PORT_H;
    processMessage(&Serial, source, dest, payloadSize, sysCommand, body);
    delete[] body;
    body = NULL;
  }

  for (int i = 0; i < 7; i++) {
    SoftwareSerial *port = ALL_PORTS[i];
    port->listen();
    if (hasIncoming(port)) {
      Serial.println("recieving message");
      readMessage(port, source, dest, payloadSize, sysCommand, body);
      Serial.write((char *)body, payloadSize);
      Serial.println();
      processMessage(port, source, dest, payloadSize, sysCommand, body);
      delete[] body;
      body = NULL;
    }
  }
}

void processMessage(Stream *srcPort, NodeId_t source, NodeId_t dest, MessageSize_t payloadSize, SysCommand_t sysCommand, byte *message) {
  char buff[100];
  if (sysCommand & GET_ID) {
    // Get ID is usually only used when the sender doesn't know the ID of the node, so just send it back the same srcPort
    sprintf(buff, "Node ID requested by %hu", source);
    Serial.println(buff);
    srcPort->write((char *) &NODE_ID, sizeof(NODE_ID));
    return;
  }
  if (dest != NODE_ID) {
    routeMessage(srcPort, source, dest, payloadSize, sysCommand, message);
    return;
  }
  if (sysCommand & UPDATE_NEIGHBORS) {

  }
  if (sysCommand & GET_NEIGHBORS) {
    sprintf(buff, "Node Neighbors requested by %hu", source);
    Serial.println(buff);
    resetNeigbors();
    routeMessage(srcPort, dest, source, sizeof(neighborIds), UPDATE_NEIGHBORS, (byte *) neighborIds);
  }
  if (sysCommand & GET_ROUTING) {
    sprintf(buff, "Network Topology requested by %hu", source);
    Serial.println(buff);
//    byte *serialized = NULL;
//    const MessageSize_t topologySize = serializeRoutingTable(routingTable, serialized);
//    routeMessage(srcPort, dest, source, topologySize, UPDATE_ROUTING, topology);
  }
  if (sysCommand & UPDATE_ROUTING) {
    sprintf(buff, "Network Topology update from %hu", source);
    Serial.println(buff);
  }
  if (sysCommand & DISCOVER_TOPOLOGY) {
    sprintf(buff, "Network Topology discovery triggered by %hu", source);
    Serial.println(buff);
    startDiscovery();
  }
}

void routeMessage(Stream *srcPort, NodeId_t source, NodeId_t dest, MessageSize_t payloadSize, SysCommand_t sysCommand, byte *message) {
  char buff[100];
  sprintf(buff, "Routing message from %hu to %hu via %hu size %hu", source, dest, NODE_ID, payloadSize);
  Serial.println(buff);
  if (dest == PORT_H) {
    writeMessage(&Serial, source, PORT_H, payloadSize, sysCommand, message);
    return;
  }
}

void resetNeigbors() {
  Serial.println("Reset Edges");
  int maxRetries = 2 * (LISTEN_WAIT * 8 / PING_DELAY);
  for (int i = 0; i < 6; i++) {
    NEIGHBORS[i]->listen();
    if (ackWait(NEIGHBORS[i], maxRetries)) {
      writeMessage(NEIGHBORS[i], NODE_ID, EMPTY, 0, GET_ID, NULL);
      NodeId_t neighbor = EMPTY;
      NEIGHBORS[i]->readBytes((byte *) &neighbor, sizeof(NodeId_t));
      neighborIds[i] = neighbor;
    } else {
      neighborIds[i] = EMPTY;
    }
  }
}

void startDiscovery() {
  Serial.println("Purge queue");
  discoveryQueue.purge();
  Serial.println("Purge visited");
  discoverVisited.purge();
  Serial.println("Purge edges");
  topology.purge();
  discoveryDone = false;

  char buff[100] = { 0 };
  sprintf(buff, "collection sizes %u %u %u", discoveryQueue.count, discoverVisited.count, topology.adj.values.count);
  Serial.println(buff);

  resetNeigbors();
  updateNetwork(NODE_ID, neighborIds);
}

void updateNetwork(const NodeId_t source, const NodeId_t *dest) {
  char buff[100] = { 0 };
  discoverVisited.pushBack(source);
  sprintf(buff, "%hu == %hu, %d", source, NODE_ID, discoverVisited.count);
  Serial.println(buff);
  for (int i = 0; i < 6; i++) {
    sprintf(buff, "source %hu dest %hu", source, dest[i]);
    Serial.println(buff);
    if (dest[i] != EMPTY) {
      if (!discoverVisited.contains(dest[i])) {
        discoveryQueue.pushBack(dest[i]);
      }
      topology.addEdge(source, dest[i]);
    }
  }

  sprintf(buff, "UPDATE collection sizes %u %u %u", discoveryQueue.count, discoverVisited.count, topology.adj.values.count);
  Serial.println(buff);

  distributeTopology();

  //  if (discoveryQueue.isEmpty()) {
  //    discoveryDone = true;
  //    return;
  //  }
  //
  //  while (!discoveryQueue.isEmpty()) {
  //    NodeId_t next = discoveryQueue.popFront();
  //    routeMessage(NULL, NODE_ID, next, 0, GET_NEIGHBORS, NULL);
  //  }
}

void distributeTopology() {
  byte *message = NULL;
//  const MessageSize_t messageSize = serializeTopology(message);

  Serial.println("DISTRIBUTE TOPO");

//  Set<NodeId_t> allNodes;
//  for (LinkedNode<GraphEdge<NodeId_t>> *edge = edges.front; edge != NULL; edge = edge->next) {
//    allNodes.pushBack(edge->val.src);
//    allNodes.pushBack(edge->val.dest);
//  }
//
//  for (LinkedNode<NodeId_t> *i = allNodes.front; i != NULL; i = i->next) {
//    Serial.println(i->val, HEX);
//    if (i->val == NODE_ID) {
//      continue;
//    }
//    routeMessage(NULL, NODE_ID, i->val, messageSize, UPDATE_ROUTING, message);
//  }

  Serial.println("DISTRIBUTE TOPO DONE");

  delete[] message;
}

MessageSize_t serializeRoutingTable(const Map<NodeId_t, NodeId_t> &table, byte *&destination) {
  char buff[100] = { 0 };
  const MessageSize_t messageSize = table.values.count * sizeof(NodeId_t) * 2;
  destination = new byte[messageSize] { EMPTY };
  LinkedNode<Pair<NodeId_t, NodeId_t>> *pair = table.values.front;
  for (int i = 0; pair != NULL; i++, pair = pair->next) {
    sprintf(buff, "%#5hX <-> %#5hX", pair->val.left, pair->val.right);
    Serial.println(buff);
    int pairSize = sizeof(pair->val.left) + sizeof(pair->val.right);
    memcpy(destination + i * pairSize, (byte *) &pair->val.left, sizeof(pair->val.left));
    memcpy(destination + i * pairSize + sizeof(pair->val.left), (byte *) &pair->val.right, sizeof(pair->val.right));
  }

  return messageSize;
}

int strcicmp(char const *a, char const *b)
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

void setStatusLed(__uint24 grb, int duration) {
  pixels.clear();
  pixels.setBrightness(BRIGHTNESS);
  pixels.setPixelColor(STATUS_IDX, grb);
  pixels.show();
  if (duration > 0) {
    delay(duration);
    pixels.setBrightness(BRIGHTNESS);
    pixels.setPixelColor(STATUS_IDX, 0x000000);
    pixels.show();
  }
}

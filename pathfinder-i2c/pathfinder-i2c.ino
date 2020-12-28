#include <Wire.h>

#include "DataStructures.h"
#include "CommonMessaging.h"
#include "PathFinder.h"

Graph<NodeId_t> topology(true, 0, EEPROM.length());
Set<NodeId_t> discoveryVisited;
LinkedList<NodeId_t> discoveryQueue;
LinkedList<NodeId_t> discoveryUnexplored;

bool discoveryDone = true;
byte *message = NULL;
int receivedLen = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(FINDER_I2C_ADDR);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Serial.println("Start");
}

void loop() {
}

void onReceive(int numBytes) {
  Serial.print("recieved ");
  while (Wire.available() < numBytes);
  message = new byte[numBytes];
  Wire.readBytes(message, numBytes);
  receivedLen = numBytes;
  bool processed = false;

  for (int i = 0; i < numBytes; i++) {
    Serial.print(message[i], HEX);
    Serial.print(", ");
  }
  Serial.println();

  // Process requests that do not require data return
  byte *body = message + 1;
  byte command = message[0];
  NodeId_t node;
  if (command == FINDER_ADD_NODE) {
    Serial.println("Add Node");
    node = ((NodeId_t *) body)[0];
    size_t numNodes = ((size_t *) (body + sizeof(node)))[0];
    NodeId_t *neighbors = ((NodeId_t *) (body + sizeof(node) + sizeof(numNodes)));
    addNode(node, neighbors, numNodes);
    processed = true;
  } else if (command == FINDER_START_DISCOVERY) {
    Serial.println("Start Discovery");
    node = ((NodeId_t *) body)[0];
    startDiscovery(node);
    processed = true;
  } else if (command == FINDER_CLEAR_TOPOLOGY) {
    Serial.println("Clearing topology");
    clearTopology();
    processed = true;
  }

  if (processed) {
    clearMessage();
  }
}

void onRequest() {
  if (receivedLen) {
    if (message[0] == FINDER_GET_DISCOVERY_STATS) {
      writeDiscoveryStats();
    }
    clearMessage();
  }
}

void clearMessage() {
  Serial.println("clearing message");
  receivedLen = 0;
  delete[] message;
  message = NULL;
}

void addNode(NodeId_t node, NodeId_t *neighbors, size_t numNodes) {
  for (int i = 0; i < numNodes; i++) {
    Serial.print("Adding ");
    Serial.print(node, HEX);
    Serial.print("->");
    Serial.println(neighbors[i], HEX);
    topology.addEdge(node, neighbors[i]);
  }
}

void startDiscovery(NodeId_t startNode) {
  clearTopology();
  discoveryVisited.purge();
  discoveryQueue.purge();
  discoveryUnexplored.purge();
  discoveryDone = false;
}

void clearTopology() {
  topology.purge();
}

void writeDiscoveryStats() {
  size_t numEdges;
  size_t numNodes;
  numNodes = topology.numNodes();
  numEdges = topology.numEdges();

  Serial.print("num edges: ");
  Serial.println(numEdges);
  Serial.print("num nodes: ");
  Serial.println(numNodes);

  Wire.write((byte *)&discoveryDone, sizeof(bool));
  Wire.write((byte *)&numNodes, sizeof(size_t));
  Wire.write((byte *)&numEdges, sizeof(size_t));
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

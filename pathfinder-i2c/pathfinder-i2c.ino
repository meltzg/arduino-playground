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
  byte *body = message + 1;
  Wire.readBytes(message, numBytes);
  receivedLen = numBytes;
  bool processed = false;

  for (int i = 0; i < numBytes; i++) {
    Serial.print(message[i], HEX);
    Serial.print(", ");
  }
  Serial.println();

  // Process requests that do not require data return
  NodeId_t node;
  switch (message[0]) {
    case FINDER_ADD_NODE:
      node = ((NodeId_t *) body)[0];
      size_t numNodes = ((size_t *) (body + sizeof(node)))[0];
      NodeId_t *neighbors = ((NodeId_t *) (body + sizeof(node) + sizeof(numNodes)));
      addNode(node, neighbors, numNodes);
      processed = true;
      break;
    case FINDER_START_DISCOVERY:
      node = ((NodeId_t *) body)[0];
      startDiscovery(node);
      processed = true;
      break;
    case FINDER_CLEAR_TOPOLOGY:
      clearTopology();
      processed = true;
      break;
  }

  if (processed) {
    clearMessage();
  }
}

void onRequest() {
  if (receivedLen) {
    Wire.write(message, receivedLen);
    clearMessage()
  }
}

void clearMessage() {
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
  Serial.print("num edges: ");
  Serial.println(topology.numEdges());
  Serial.print("num nodes: ");
  Serial.println(topology.numNodes());
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

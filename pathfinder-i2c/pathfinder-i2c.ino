#include <Wire.h>

#include "DataStructures.h"
#include "CommonMessaging.h"
#include "PathFinder.h"

Graph<NodeId_t> topology(true, 0, EEPROM.length());
Set<NodeId_t> discoveryVisited;
LinkedList<NodeId_t> discoveryQueue;
GraphIterator<NodeId_t> *iterator = NULL;

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
  Serial.print("In ");
  while (Wire.available() < numBytes);
  message = new byte[numBytes];
  Wire.readBytes(message, numBytes);
  receivedLen = numBytes;
  bool processed = false;

  for (int i = 0; i < numBytes; i++) {
    Serial.print(message[i], HEX);
    Serial.print(",");
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
    startDiscovery();
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
    byte command = message[0];
    byte *body = message + 1;
    if (command == FINDER_GET_DISCOVERY_STATS) {
      Serial.println("Get stats");
      writeDiscoveryStats();
    } else if (command == FINDER_GET_NEIGHBOR_REQUEST) {
      Serial.println("Get neighbor request");
      writeNeighborRequest();
    } else if (command == FINDER_GET_NEXT_STEP) {
      Serial.println("Get next step");
      NodeId_t src = ((NodeId_t *) body)[0];
      NodeId_t dest = ((NodeId_t *) body)[1];
      writeNextStep(src, dest);
    }
    clearMessage();
  }
}

void clearMessage() {
  receivedLen = 0;
  delete[] message;
  message = NULL;
}

void addNode(NodeId_t node, NodeId_t *neighbors, size_t numNodes) {
  if (node != EMPTY) {
    for (int i = 0; i < numNodes; i++) {
      Serial.print("A ");
      Serial.print(node);
      Serial.print("->");
      Serial.println(neighbors[i]);
      if (neighbors[i] == EMPTY) {
        continue;
      }
      topology.addEdge(node, neighbors[i]);
    }
  } else {
    return;
  }

  if (!discoveryDone) {
    discoveryVisited.pushBack(node);
    for (int i = 0; i < numNodes; i++) {
      if (!discoveryVisited.contains(neighbors[i])) {
        discoveryQueue.pushBack(neighbors[i]);
        discoveryVisited.pushBack(neighbors[i]);
      }
    }
  }

  if (discoveryQueue.isEmpty()) {
    Serial.println("Discovery done");
    discoveryDone = true;
    discoveryVisited.purge();

    for (GraphIterator<NodeId_t> iter(topology, node); iter.hasNext();) {
      NodeId_t curr = iter.next();
      Serial.print(curr, HEX);
      Serial.print(": ");
      Set<NodeId_t> adj;
      topology.getAdjacent(curr, adj);
      for (ListIterator<NodeId_t> aIter(adj); aIter.hasNext();) {
        Serial.print(aIter.next(), HEX);
        Serial.print(", ");
      }
      Serial.println();
    }
  }
}

void startDiscovery() {
  clearTopology();
  discoveryVisited.purge();
  discoveryQueue.purge();
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

  Serial.print("Es ");
  Serial.println(numEdges);
  Serial.print("Ns ");
  Serial.println(numNodes);

  Wire.write((byte *)&discoveryDone, sizeof(bool));
  Wire.write((byte *)&numNodes, sizeof(size_t));
  Wire.write((byte *)&numEdges, sizeof(size_t));
}

void writeNeighborRequest() {
  NodeId_t nextNeighbor = EMPTY;
  if (!discoveryQueue.isEmpty()) {
    nextNeighbor = discoveryQueue.popFront();
  }

  Serial.print("req ");
  Serial.println(nextNeighbor, HEX);

  Wire.write((byte *)&nextNeighbor, sizeof(NodeId_t));
}

void writeNextStep(NodeId_t src, NodeId_t dest) {
  Serial.print(src, HEX);
  Serial.print("->");
  Serial.println(dest, HEX);
  LinkedList<NodeId_t> path;
  NodeId_t nextStep = EMPTY;

  topology.getShortestPath(src, dest, path);
  if (!path.isEmpty()) {
    int i = 0;
    for (ListIterator<NodeId_t> iter(path); iter.hasNext(); i++) {
      NodeId_t node = iter.next();
      if (i == 1) {
        nextStep = node;
      }
      Serial.print(node, HEX);
      Serial.print(",");
    }
    Serial.println();
  }
  Wire.write((byte *)&nextStep, sizeof(NodeId_t));
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

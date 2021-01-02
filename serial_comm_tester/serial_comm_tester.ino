#include <Wire.h>
#include "CommonMessaging.h"
#include "PathFinder.h"
#include "DataStructures.h"

PathFinder p;

void setup() {
  long startTs = millis();
  Serial.begin(9600);
  Wire.begin();
  Serial.print("Start: ");
  Serial.println(startTs);
  Graph<NodeId_t> g(true, 0, EEPROM.length());

  g.addEdge(1, 2);
  g.addEdge(1, 5);
  g.addEdge(1, 6);
  g.addEdge(2, 3);
  g.addEdge(2, 6);
  g.addEdge(2, 7);
  g.addEdge(3, 4);
  g.addEdge(3, 7);
  g.addEdge(3, 8);
  g.addEdge(4, 8);
  g.addEdge(4, 9);
  g.addEdge(5, 6);
  g.addEdge(5, 10);
  g.addEdge(5, 11);
  g.addEdge(6, 7);
  g.addEdge(6, 11);
  g.addEdge(6, 12);
  g.addEdge(7, 8);
  g.addEdge(7, 12);
  g.addEdge(7, 13);
  g.addEdge(8, 9);
  g.addEdge(8, 13);
  g.addEdge(8, 14);
  g.addEdge(9, 14);
  g.addEdge(9, 15);
  g.addEdge(10, 11);
  g.addEdge(10, 16);
  g.addEdge(10, 17);
  g.addEdge(11, 12);
  g.addEdge(11, 17);
  g.addEdge(11, 18);
  g.addEdge(12, 13);
  g.addEdge(12, 18);
  g.addEdge(12, 19);
  g.addEdge(13, 14);
  g.addEdge(13, 19);
  g.addEdge(13, 20);
  g.addEdge(14, 15);
  g.addEdge(14, 20);
  g.addEdge(14, 21);
  g.addEdge(15, 21);
  g.addEdge(15, 22);
  g.addEdge(16, 17);
  g.addEdge(16, 23);
  g.addEdge(16, 24);
  g.addEdge(17, 18);
  g.addEdge(17, 24);
  g.addEdge(17, 25);
  g.addEdge(18, 19);
  g.addEdge(18, 25);
  g.addEdge(18, 26);
  g.addEdge(19, 20);
  g.addEdge(19, 26);
  g.addEdge(19, 27);
  g.addEdge(20, 21);
  g.addEdge(20, 27);
  g.addEdge(20, 28);
  g.addEdge(21, 22);
  g.addEdge(21, 28);
  g.addEdge(21, 29);
  g.addEdge(22, 29);
  g.addEdge(22, 30);
  g.addEdge(23, 24);
  g.addEdge(23, 31);
  g.addEdge(24, 25);
  g.addEdge(24, 32);
  g.addEdge(24, 31);
  g.addEdge(25, 26);
  g.addEdge(25, 33);
  g.addEdge(25, 32);
  g.addEdge(26, 27);
  g.addEdge(26, 34);
  g.addEdge(26, 33);
  g.addEdge(27, 28);
  g.addEdge(27, 35);
  g.addEdge(27, 34);
  g.addEdge(28, 29);
  g.addEdge(28, 36);
  g.addEdge(28, 35);
  g.addEdge(29, 30);
  g.addEdge(29, 37);
  g.addEdge(29, 36);
  g.addEdge(30, 37);
  g.addEdge(31, 32);
  g.addEdge(31, 38);
  g.addEdge(32, 33);
  g.addEdge(32, 39);
  g.addEdge(32, 38);
  g.addEdge(33, 34);
  g.addEdge(33, 40);
  g.addEdge(33, 39);
  g.addEdge(34, 35);
  g.addEdge(34, 41);
  g.addEdge(34, 40);
  g.addEdge(35, 36);
  g.addEdge(35, 42);
  g.addEdge(35, 41);
  g.addEdge(36, 37);
  g.addEdge(36, 43);
  g.addEdge(36, 42);
  g.addEdge(37, 43);
  g.addEdge(38, 39);
  g.addEdge(38, 44);
  g.addEdge(39, 40);
  g.addEdge(39, 45);
  g.addEdge(39, 44);
  g.addEdge(40, 41);
  g.addEdge(40, 46);
  g.addEdge(40, 45);
  g.addEdge(41, 42);
  g.addEdge(41, 47);
  g.addEdge(41, 46);
  g.addEdge(42, 43);
  g.addEdge(42, 48);
  g.addEdge(42, 47);
  g.addEdge(43, 48);
  g.addEdge(44, 45);
  g.addEdge(44, 49);
  g.addEdge(45, 46);
  g.addEdge(45, 50);
  g.addEdge(45, 49);
  g.addEdge(46, 47);
  g.addEdge(46, 51);
  g.addEdge(46, 50);
  g.addEdge(47, 48);
  g.addEdge(47, 52);
  g.addEdge(47, 51);
  g.addEdge(48, 52);
  g.addEdge(49, 50);
  g.addEdge(50, 51);
  g.addEdge(51, 52);

  Serial.print("Es ");
  Serial.println(g.numEdges());
  Serial.print("Ns ");
  Serial.println(g.numNodes());

  LinkedList<NodeId_t> path;
  g.getShortestPath(1, 52, path);
  Serial.print("Path: ");
  for (ListIterator<NodeId_t> iter(path); iter.hasNext();) {
    Serial.print(iter.next());
    Serial.print(", ");
  }
  Serial.println();

  p.startDiscovery();
  Set<NodeId_t> neighbors;
  NodeId_t *neiArr = NULL;
  NodeId_t currNode = 1;

  do {
    Serial.println(currNode);
    g.getAdjacent(currNode, neighbors);
    delete[] neiArr;
    neiArr  = new NodeId_t[neighbors.count];
    int i = 0;
    for (ListIterator<NodeId_t> iter(neighbors); iter.hasNext(); i++) {
      neiArr[i] = iter.next();
    }
    p.addNode(currNode, neiArr, neighbors.count);
    currNode = p.getNextNeighborRequest();
  } while (!p.getDiscoveryStats().discoveryDone && currNode != EMPTY);

  DiscoveryStats stats = p.getDiscoveryStats();
  printDiscoveryStats(stats);

  Serial.print("Next Step: ");
  Serial.print(p.getNextStep(1, 52));
  Serial.println();

  Serial.println("Validate pathfinding");
  int progress = 0;
  int total = 52 * 51;
  bool error = false;
  for (NodeId_t i = 1; i < 53; i++) {
    NodeId_t nextStep = EMPTY;
    for (NodeId_t j = 1; j < 53; j++) {
      if (i == j) {
        continue;
      }
      LinkedList<NodeId_t> path;
      g.getShortestPath(i, j, path);
      NodeId_t expected = EMPTY;
      if (!path.isEmpty()) {
        ListIterator<NodeId_t> iter(path);
        iter.next();
        expected = iter.next();
      }
      NodeId_t nextStep = p.getNextStep(i, j);
      Serial.print("Progress: ");
      Serial.print(++progress);
      Serial.print("/");
      Serial.println(total);
      if (nextStep == EMPTY) {
        Serial.print("Path not found from ");
        Serial.print(i);
        Serial.print(" to ");
        Serial.print(j);
        Serial.print(". expected ");
        Serial.println(expected);
        error = true;
        break;
      }
    }
    if (error) {
      break;
    }
  }

  Serial.println("Done: ");
  Serial.println(millis() - startTs);

}

void loop() {
}

void printDiscoveryStats(const DiscoveryStats &stats) {
  Serial.print("discovery done: ");
  Serial.print(stats.discoveryDone);
  Serial.print(" num nodes: ");
  Serial.print(stats.numNodes);
  Serial.print(" num edges: ");
  Serial.println(stats.numEdges);
}

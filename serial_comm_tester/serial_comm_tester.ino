#include <Wire.h>
#include "CommonMessaging.h"
#include "PathFinder.h"

PathFinder p;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Start");
  NodeId_t node = 0x01ab;
  NodeId_t neighbors[3] = { 0x02ab, 0x03ab, 0x04ab };
  p.addNode(node, neighbors, 3);
  printDiscoveryStats(p.getDiscoveryStats());
  p.clearTopology();
  printDiscoveryStats(p.getDiscoveryStats());
}

void loop() {
  while (Serial.available()) {
    String str = Serial.readStringUntil('\n');
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(str.c_str());
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, str.length());    // request 6 bytes from slave device #8

    while (Wire.available()) { // slave may send less than requested
      size_t count = Wire.available();
      char msg[count + 1] = { 0 };
      Wire.readBytes(msg, count); // receive a byte as character
      Serial.write(msg, count);         // print the character
    }
    Serial.println();
  }
}

void printDiscoveryStats(const DiscoveryStats &stats) {
  Serial.print("discovery done: ");
  Serial.print(stats.discoveryDone);
  Serial.print(" num nodes: ");
  Serial.print(stats.numNodes);
  Serial.print(" num edges: ");
  Serial.println(stats.numEdges, HEX);
}

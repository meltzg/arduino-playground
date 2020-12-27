#include <Wire.h>

#include "DataStructures.h"
#include "CommonMessaging.h"
#include "PathFinder.h"

Graph<NodeId_t> topology(true, 0, EEPROM.length());
Set<NodeId_t> discoveryVisited;
LinkedList<NodeId_t> discoveryQueue;
LinkedList<NodeId_t> discoveryUnexplored;

bool discoveryDone = true;
byte *received = NULL;
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
  Serial.println(numBytes);
  while (Wire.available() < numBytes);
  received = new byte[numBytes];
  Wire.readBytes(received, numBytes);
  receivedLen = numBytes;
}

void onRequest() {
  if (receivedLen) {
    Wire.write(received, receivedLen);
    receivedLen = 0;
    delete[] received;
    received = NULL;
  }
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

#include "DataStructures.h"
#include "CommonMessaging.h"

// Pathfinder commands
#define FINDER_ADD_NODE 0x01
#define FINDER_GET_PATH 0x02
#define FINDER_START_DISCOVERY 0x03
#define FINDER_GET_DISCOVERY_STATS 0x04
#define FINDER_GET_NEIGHBOR_REQUEST 0x05
#define FINDER_ITERATOR_NEXT 0x06
#define FINDER_ITERATE_NEXT 0x07


Graph<NodeId_t> topology(true, 0, EEPROM.length());
Set<NodeId_t> discoveryVisited;
LinkedList<NodeId_t> discoveryQueue;
LinkeList<NodeId_t> discoveryUnexplored;

bool discoveryDone = true;

void setup() {
  Serial.begin(9600);
  Serial.println("Start");
}

void loop() {
  // put your main code here, to run repeatedly:
  //  Serial.println(freeMemory());
  //  delay(500);
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

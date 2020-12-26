#include "DataStructures.h"
#include "CommonMessaging.h"

Graph<NodeId_t> topology(true, 0, EEPROM.length());
char buf[10];

void setup() {
  Serial.begin(9600);
  Serial.println("Start");
  Serial.println(freeMemory());
  topology.addEdge(0, 1);
  topology.addEdge(0, 4);
  topology.addEdge(0, 5);
  topology.addEdge(1, 2);
  topology.addEdge(1, 5);
  topology.addEdge(1, 6);
  topology.addEdge(2, 3);
  topology.addEdge(2, 6);
  topology.addEdge(2, 7);
  topology.addEdge(3, 7);
  topology.addEdge(3, 8);
  topology.addEdge(4, 5);
  topology.addEdge(4, 9);
  topology.addEdge(4, 10);
  topology.addEdge(5, 6);
  topology.addEdge(5, 10);
  topology.addEdge(5, 11);
  topology.addEdge(6, 7);
  topology.addEdge(6, 11);
  topology.addEdge(6, 12);
  topology.addEdge(7, 8);
  topology.addEdge(7, 12);
  topology.addEdge(7, 13);
  topology.addEdge(8, 13);
  topology.addEdge(8, 14);
  topology.addEdge(9, 10);
  topology.addEdge(9, 15);
  topology.addEdge(9, 16);
  topology.addEdge(10, 11);
  topology.addEdge(10, 16);
  topology.addEdge(10, 17);
  topology.addEdge(11, 12);
  topology.addEdge(11, 17);
  topology.addEdge(11, 18);
  topology.addEdge(12, 13);
  topology.addEdge(12, 18);
  topology.addEdge(12, 19);
  topology.addEdge(13, 14);
  topology.addEdge(13, 19);
  topology.addEdge(13, 20);
  topology.addEdge(14, 20);
  topology.addEdge(14, 21);
  topology.addEdge(15, 16);
  topology.addEdge(15, 22);
  topology.addEdge(15, 23);
  topology.addEdge(16, 17);
  topology.addEdge(16, 23);
  topology.addEdge(16, 24);
  topology.addEdge(17, 18);
  topology.addEdge(17, 24);
  topology.addEdge(17, 25);
  topology.addEdge(18, 19);
  topology.addEdge(18, 25);
  topology.addEdge(18, 26);
  topology.addEdge(19, 20);
  topology.addEdge(19, 26);
  topology.addEdge(19, 27);
  topology.addEdge(20, 21);
  topology.addEdge(20, 27);
  topology.addEdge(20, 28);
  topology.addEdge(21, 28);
  topology.addEdge(21, 29);
  topology.addEdge(22, 23);
  topology.addEdge(22, 30);
  topology.addEdge(23, 24);
  topology.addEdge(23, 31);
  topology.addEdge(23, 30);
  topology.addEdge(24, 25);
  topology.addEdge(24, 32);
  topology.addEdge(24, 31);
  topology.addEdge(25, 26);
  topology.addEdge(25, 33);
  topology.addEdge(25, 32);
  topology.addEdge(26, 27);
  topology.addEdge(26, 34);
  topology.addEdge(26, 33);
  topology.addEdge(27, 28);
  topology.addEdge(27, 35);
  topology.addEdge(27, 34);
  topology.addEdge(28, 29);
  topology.addEdge(28, 36);
  topology.addEdge(28, 35);
  topology.addEdge(29, 36);
  topology.addEdge(30, 31);
  topology.addEdge(30, 37);
  topology.addEdge(31, 32);
  topology.addEdge(31, 38);
  topology.addEdge(31, 37);
  topology.addEdge(32, 33);
  topology.addEdge(32, 39);
  topology.addEdge(32, 38);
  topology.addEdge(33, 34);
  topology.addEdge(33, 40);
  topology.addEdge(33, 39);
  topology.addEdge(34, 35);
  topology.addEdge(34, 41);
  topology.addEdge(34, 40);
  topology.addEdge(35, 36);
  topology.addEdge(35, 42);
  topology.addEdge(35, 41);
  topology.addEdge(36, 42);
  topology.addEdge(37, 38);
  topology.addEdge(37, 43);
  topology.addEdge(38, 39);
  topology.addEdge(38, 44);
  topology.addEdge(38, 43);
  topology.addEdge(39, 40);
  topology.addEdge(39, 45);
  topology.addEdge(39, 44);
  topology.addEdge(40, 41);
  topology.addEdge(40, 46);
  topology.addEdge(40, 45);
  topology.addEdge(41, 42);
  topology.addEdge(41, 47);
  topology.addEdge(41, 46);
  topology.addEdge(42, 47);
  topology.addEdge(43, 44);
  topology.addEdge(43, 48);
  topology.addEdge(44, 45);
  topology.addEdge(44, 49);
  topology.addEdge(44, 48);
  topology.addEdge(45, 46);
  topology.addEdge(45, 50);
  topology.addEdge(45, 49);
  topology.addEdge(46, 47);
  topology.addEdge(46, 51);
  topology.addEdge(46, 50);
  topology.addEdge(47, 51);
  topology.addEdge(48, 49);
  topology.addEdge(49, 50);
  topology.addEdge(50, 51);
  Serial.println(freeMemory());
  doAllShortestPaths();
  Serial.println(freeMemory());
  Serial.println("Done");

}

void loop() {
  // put your main code here, to run repeatedly:
  //  Serial.println(freeMemory());
  //  delay(500);
}

void doAllShortestPaths() {
  long paths = 0;
  for (int src = 0; src < 52; src++) {
    for (int dest = 0; dest < 52; dest++) {
      if (src == dest) {
        continue;
      }
      Serial.print("Paths: ");
      Serial.println(++paths, DEC);
      LinkedList<NodeId_t> path;
      topology.getShortestPath(src, dest, path);
      if (path.isEmpty()) {
        sprintf(buf, "src: %d ", src);
        Serial.print(buf);
        sprintf(buf, "dest: %d ", dest);
        Serial.print(buf);
        return;
      }
    }
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

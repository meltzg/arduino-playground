#include <Wire.h>
#include "CommonMessaging.h"
#include "PathFinder.h"
#include "DataStructures.h"

PathFinder p(6);

void setup()
{
    long startTs = millis();
    Serial.begin(9600);
    Wire.begin();
    Serial.print(F("Start: "));
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

    Serial.print(F("Es "));
    Serial.println(g.numEdges());
    Serial.print(F("Ns "));
    Serial.println(g.numNodes());

    LinkedList<NodeId_t> path;
    g.getShortestPath(1, 52, path);
    Serial.print(F("Path: "));
    for (ListIterator<NodeId_t> iter(path); iter.hasNext();)
    {
        Serial.print(iter.next());
        Serial.print(F(", "));
    }
    Serial.println();

    p.startDiscovery();
    Set<NodeId_t> neighbors;
    NodeId_t *neiArr = NULL;
    NodeId_t currNode = 1;

    Serial.println(F("Test async discovery"));
    do
    {
        Serial.println(currNode);
        g.getAdjacent(currNode, neighbors);
        delete[] neiArr;
        neiArr = new NodeId_t[neighbors.count];
        int i = 0;
        for (ListIterator<NodeId_t> iter(neighbors); iter.hasNext(); i++)
        {
            neiArr[i] = iter.next();
        }
        p.addNode(currNode, neiArr, neighbors.count);
        p.resetIterator(1);
        for (NodeId_t distribId = p.getIteratorNext(); distribId != EMPTY; distribId = p.getIteratorNext())
        {
            Serial.print(F("sim update existing "));
            Serial.println(p.getNextStep(1, distribId));
        }
        p.resetIterator(1);
        for (NodeId_t distribId = p.getIteratorNext(); distribId != EMPTY; distribId = p.getIteratorNext())
        {
            Serial.print(F("sim initialize new "));
            Serial.print(distribId);
            Serial.print(F(" "));
            Set<NodeId_t> adj;
            p.getAdjacent(distribId, adj);
            ListIterator<NodeId_t> adjIter(adj);
            for (int i = 1; adjIter.hasNext(); i++)
            {
                NodeId_t a = adjIter.next();
                Serial.print(a);
                Serial.print(F(", "));
            }
            Serial.println();
            for (int i = 0; i < neighbors.count; i++)
            {
                Serial.println(p.getNextStep(1, neiArr[i]));
            }
        }
        currNode = p.getNextNeighborRequest();
    } while (!p.getDiscoveryStats().discoveryDone && currNode != EMPTY);

    DiscoveryStats stats = p.getDiscoveryStats();
    printDiscoveryStats(stats);

    Serial.println(F("Validate iteration"));
    p.resetIterator(1);
    for (NodeId_t next = p.getIteratorNext(); next != EMPTY; next = p.getIteratorNext())
    {
        Set<NodeId_t> adj;
        p.getAdjacent(next, adj);
        Serial.print(next);
        Serial.print(F(": "));
        for (ListIterator<NodeId_t> iter(adj); iter.hasNext();)
        {
            Serial.print(iter.next());
            Serial.print(F(", "));
        }
        Serial.println();
    }

    Serial.println(F("Done: "));
    Serial.println(millis() - startTs);
}

void loop()
{
}

void printDiscoveryStats(const DiscoveryStats &stats)
{
    Serial.print(F("discovery done: "));
    Serial.print(stats.discoveryDone);
    Serial.print(F(" num nodes: "));
    Serial.print(stats.numNodes);
    Serial.print(F(" num edges: "));
    Serial.println(stats.numEdges);
}

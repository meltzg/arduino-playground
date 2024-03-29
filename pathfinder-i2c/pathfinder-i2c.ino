#include <Wire.h>

#include "DataStructures.h"
#include "CommonMessaging.h"
#include "PathFinder.h"

Graph<NodeId_t> topology(true, 0, EEPROM.length(), false);
Set<NodeId_t> discoveryVisited;
LinkedList<NodeId_t> discoveryQueue;
GraphIterator<NodeId_t> iterator;

bool discoveryDone = true;
byte *message = NULL;
int receivedLen = 0;

void setup()
{
    Serial.begin(9600);
    Wire.begin(FINDER_I2C_ADDR);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Serial.println(F("Start"));
    size_t numEdges;
    size_t numNodes;
    numNodes = topology.numNodes();
    numEdges = topology.numEdges();

    Serial.print(F("Es "));
    Serial.println(numEdges);
    Serial.print(F("Ns "));
    Serial.println(numNodes);
}

void loop()
{
}

void onReceive(int numBytes)
{
    clearMessage();
    Serial.print(F("Free mem "));
    Serial.println(freeMemory());
    Serial.print(F("In expect "));
    Serial.print(numBytes);
    Serial.print(F(" bytes "));
    while (Wire.available() < numBytes)
    {
        Serial.print(F("available"));
        Serial.println(Wire.available());
        Serial.flush();
    }
    message = new byte[numBytes];
    Wire.readBytes(message, numBytes);
    receivedLen = numBytes;
    bool processed = false;

    for (int i = 0; i < numBytes; i++)
    {
        Serial.print(message[i], HEX);
        Serial.print(F(","));
    }
    Serial.println();
    Serial.flush();
}

void onRequest()
{
    if (receivedLen)
    {
        byte command = message[0];
        byte *body = message + 1;
        if (command == FINDER_ADD_NODE)
        {
            NodeId_t node = ((NodeId_t *)body)[0];
            size_t numNodes = ((size_t *)(body + sizeof(node)))[0];
            NodeId_t *neighbors = ((NodeId_t *)(body + sizeof(node) + sizeof(numNodes)));
            addNode(node, neighbors, numNodes);
        }
        else if (command == FINDER_START_DISCOVERY)
        {
            startDiscovery();
        }
        else if (command == FINDER_CLEAR_TOPOLOGY)
        {
            clearTopology();
        }
        else if (command == FINDER_ITERATOR_CLEAR)
        {
            iterator.clear();
        }
        else if (command == FINDER_ITERATOR_RESET)
        {
            NodeId_t node = ((NodeId_t *)body)[0];
            iterator.reset(topology, node);
        }
        else if (command == FINDER_GET_DISCOVERY_STATS)
        {
            writeDiscoveryStats();
        }
        else if (command == FINDER_GET_NEIGHBOR_REQUEST)
        {
            writeNeighborRequest();
        }
        else if (command == FINDER_GET_NEXT_STEP)
        {
            NodeId_t src = ((NodeId_t *)body)[0];
            NodeId_t dest = ((NodeId_t *)body)[1];
            writeNextStep(src, dest);
        }
        else if (command == FINDER_ITERATOR_NEXT)
        {
            writeIteratorNext();
        }
        else if (command == FINDER_GET_ADJACENT)
        {
            NodeId_t node = ((NodeId_t *)body)[0];
            writeAdjacent(node);
        }
        // to prevent timing issues, clients should always expect 1 byte of data before continuing execution
        Wire.write(0x00);
        clearMessage();
        Serial.println(F("***"));
    }
}

void clearMessage()
{
    receivedLen = 0;
    delete[] message;
    message = NULL;
}

void addNode(NodeId_t node, NodeId_t *neighbors, size_t numNodes)
{
    if (node != EMPTY)
    {
        for (int i = 0; i < numNodes; i++)
        {
            Serial.print(F("A "));
            Serial.print(node, HEX);
            Serial.print(F("->"));
            Serial.println(neighbors[i], HEX);
            if (neighbors[i] == EMPTY)
            {
                continue;
            }
            topology.addEdge(node, neighbors[i]);
        }
    }
    else
    {
        return;
    }

    if (!discoveryDone)
    {
        discoveryVisited.pushBack(node);
        for (int i = 0; i < numNodes; i++)
        {
            if (neighbors[i] != EMPTY && !discoveryVisited.contains(neighbors[i]))
            {
                discoveryQueue.pushBack(neighbors[i]);
                discoveryVisited.pushBack(neighbors[i]);
            }
        }
    }

    if (discoveryQueue.isEmpty())
    {
        Serial.println(F("Discovery done"));
        discoveryDone = true;
        discoveryVisited.purge();

        for (GraphIterator<NodeId_t> iter(topology, node); iter.hasNext();)
        {
            NodeId_t curr = iter.next();
            Serial.print(curr, HEX);
            Serial.print(F(": "));
            Set<NodeId_t> adj;
            topology.getAdjacent(curr, adj);
            for (ListIterator<NodeId_t> aIter(adj); aIter.hasNext();)
            {
                Serial.print(aIter.next(), HEX);
                Serial.print(F(", "));
            }
            Serial.println();
        }
    }
    else
    {
        Serial.print(F("To discover: "));
        Serial.println(discoveryQueue.count);
    }
}

void startDiscovery()
{
    clearTopology();
    discoveryVisited.purge();
    discoveryQueue.purge();
    discoveryDone = false;
}

void clearTopology()
{
    topology.purge();
}

void writeDiscoveryStats()
{
    size_t numEdges;
    size_t numNodes;
    numNodes = topology.numNodes();
    numEdges = topology.numEdges();

    Serial.print(F("Es "));
    Serial.println(numEdges);
    Serial.print(F("Ns "));
    Serial.println(numNodes);

    Wire.write((byte *)&discoveryDone, sizeof(bool));
    Wire.write((byte *)&numNodes, sizeof(size_t));
    Wire.write((byte *)&numEdges, sizeof(size_t));
}

void writeNeighborRequest()
{
    NodeId_t nextNeighbor = EMPTY;
    if (!discoveryQueue.isEmpty())
    {
        nextNeighbor = discoveryQueue.popFront();
    }

    Serial.print(F("req "));
    Serial.println(nextNeighbor, HEX);
    Serial.print(F("To discover: "));
    Serial.println(discoveryQueue.count);

    Wire.write((byte *)&nextNeighbor, sizeof(NodeId_t));
}

void writeNextStep(NodeId_t src, NodeId_t dest)
{
    Serial.print(src, HEX);
    Serial.print(F("->"));
    Serial.println(dest, HEX);
    LinkedList<NodeId_t> path;
    NodeId_t nextStep = EMPTY;

    Serial.println(F("Get shortest path"));
    Serial.flush();
    topology.getShortestPath(src, dest, path);
    Serial.print(F("Path len "));
    Serial.println(path.count);
    Serial.flush();
    if (!path.isEmpty())
    {
        int i = 0;
        for (ListIterator<NodeId_t> iter(path); iter.hasNext(); i++)
        {
            NodeId_t node = iter.next();
            if (i == 1)
            {
                nextStep = node;
            }
            Serial.print(node, HEX);
            Serial.print(F(","));
        }
        Serial.println();
    }
    Wire.write((byte *)&nextStep, sizeof(NodeId_t));
}

void writeIteratorNext()
{
    NodeId_t next = EMPTY;

    if (iterator.hasNext())
    {
        next = iterator.next();
    }
    Wire.write((byte *)&next, sizeof(NodeId_t));
}

void writeAdjacent(NodeId_t node)
{
    Set<NodeId_t> adj;
    topology.getAdjacent(node, adj);

    size_t numNodes = adj.count;
    Wire.write((byte *)&numNodes, sizeof(size_t));
    for (ListIterator<NodeId_t> iter(adj); iter.hasNext();)
    {
        NodeId_t adjNode = iter.next();
        Serial.println(adjNode);
        Wire.write((byte *)&adjNode, sizeof(NodeId_t));
    }
}

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif // __arm__

int freeMemory()
{
    char top;
#ifdef __arm__
    return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
#else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif // __arm__
}

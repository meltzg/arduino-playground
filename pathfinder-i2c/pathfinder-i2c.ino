#include <Wire.h>

#include "DataStructures.h"
#include "CommonMessaging.h"
#include "PathFinder.h"

Graph<NodeId_t> topology(true, 0, EEPROM.length());
Set<NodeId_t> discoveryVisited;
Set<NodeId_t> isInitialized;
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
  Serial.println("Start");
}

void loop()
{
}

void onReceive(int numBytes)
{
  clearMessage();
  Serial.print("In ");
  while (Wire.available() < numBytes)
    ;
  message = new byte[numBytes];
  Wire.readBytes(message, numBytes);
  receivedLen = numBytes;
  bool processed = false;

  for (int i = 0; i < numBytes; i++)
  {
    Serial.print(message[i], HEX);
    Serial.print(",");
  }
  Serial.println();
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
    else if (command == FINDER_GET_INITIALIZED)
    {
      NodeId_t node = ((NodeId_t *)body)[0];
      writeIsInitialized(node);
    }
    else if (command == FINDER_SET_INITIALIZED)
    {
      NodeId_t node = ((NodeId_t *)body)[0];
      isInitialized.pushBack(node);
    }
    // to prevent timing issues, clients should always expect 1 byte of data before continuing execution
    Wire.write(0x00);
    clearMessage();
    Serial.println("***");
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
      Serial.print("A ");
      Serial.print(node, HEX);
      Serial.print("->");
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
    Serial.println("Discovery done");
    discoveryDone = true;
    discoveryVisited.purge();
    isInitialized.purge();

    for (GraphIterator<NodeId_t> iter(topology, node); iter.hasNext();)
    {
      NodeId_t curr = iter.next();
      Serial.print(curr, HEX);
      Serial.print(": ");
      Set<NodeId_t> adj;
      topology.getAdjacent(curr, adj);
      for (ListIterator<NodeId_t> aIter(adj); aIter.hasNext();)
      {
        Serial.print(aIter.next(), HEX);
        Serial.print(", ");
      }
      Serial.println();
    }
  }
  else
  {
    Serial.print("To discover: ");
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

  Serial.print("Es ");
  Serial.println(numEdges);
  Serial.print("Ns ");
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

  Serial.print("req ");
  Serial.println(nextNeighbor, HEX);
  Serial.print("To discover: ");
  Serial.println(discoveryQueue.count);

  Wire.write((byte *)&nextNeighbor, sizeof(NodeId_t));
}

void writeNextStep(NodeId_t src, NodeId_t dest)
{
  Serial.print(src, HEX);
  Serial.print("->");
  Serial.println(dest, HEX);
  LinkedList<NodeId_t> path;
  NodeId_t nextStep = EMPTY;

  topology.getShortestPath(src, dest, path);
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
      Serial.print(",");
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

void writeIsInitialized(NodeId_t node)
{
  bool isNodeInitialized = isInitialized.contains(node);
  Wire.write((byte *)&isNodeInitialized, sizeof(bool));
}

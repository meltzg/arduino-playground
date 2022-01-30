#include <Arduino.h>
#include <Wire.h>
#include "PathFinder.h"

void PathFinder::addNode(NodeId_t node, NodeId_t *neighbors, size_t numNeighbors)
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_ADD_NODE);
    Wire.write((byte *)&node, sizeof(NodeId_t));
    Wire.write((byte *)&numNeighbors, sizeof(size_t));
    Wire.write((byte *)neighbors, sizeof(NodeId_t) * numNeighbors);
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, 1);
    while (Wire.available() < 1)
    {
    }
}

NodeId_t PathFinder::getNextStep(NodeId_t src, NodeId_t dest)
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_GET_NEXT_STEP);
    Wire.write((byte *)&src, sizeof(NodeId_t));
    Wire.write((byte *)&dest, sizeof(NodeId_t));
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, sizeof(NodeId_t));
    while (Wire.available() < sizeof(NodeId_t))
    {
    }
    NodeId_t nextStep = 0;
    Wire.readBytes((byte *)&nextStep, sizeof(nextStep));
    return nextStep;
}

void PathFinder::startDiscovery()
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_START_DISCOVERY);
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, 1);
    while (Wire.available() < 1)
    {
    }
}

DiscoveryStats PathFinder::getDiscoveryStats()
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_GET_DISCOVERY_STATS);
    Wire.endTransmission();

    size_t statSize = sizeof(bool) + sizeof(size_t) * 2;
    Wire.requestFrom(FINDER_I2C_ADDR, statSize);
    while (Wire.available() < statSize)
    {
    }

    bool discoveryDone;
    size_t numNodes;
    size_t numEdges;
    Wire.readBytes((byte *)&discoveryDone, sizeof(discoveryDone));
    Wire.readBytes((byte *)&numNodes, sizeof(numNodes));
    Wire.readBytes((byte *)&numEdges, sizeof(numEdges));

    return DiscoveryStats(discoveryDone, numNodes, numEdges);
}

NodeId_t PathFinder::getNextNeighborRequest()
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_GET_NEIGHBOR_REQUEST);
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, sizeof(NodeId_t));
    while (Wire.available() < sizeof(NodeId_t))
    {
    }
    NodeId_t neighborRequest = 0;
    Wire.readBytes((byte *)&neighborRequest, sizeof(neighborRequest));
    return neighborRequest;
}

void PathFinder::resetIterator(NodeId_t start)
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_ITERATOR_RESET);
    Wire.write((byte *)&start, sizeof(start));
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, 1);
    while (Wire.available() < 1)
    {
    }
}

void clearIterator()
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_ITERATOR_CLEAR);
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, 1);
    while (Wire.available() < 1)
    {
    }
}

NodeId_t PathFinder::getIteratorNext()
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_ITERATOR_NEXT);
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, sizeof(NodeId_t));
    while (Wire.available() < sizeof(NodeId_t))
    {
    }
    NodeId_t nodeId = 0;
    Wire.readBytes((byte *)&nodeId, sizeof(nodeId));
    return nodeId;
}

void PathFinder::clearTopology()
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_CLEAR_TOPOLOGY);
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, 1);
    while (Wire.available() < 1)
    {
    }
}

void PathFinder::getAdjacent(NodeId_t node, Set<NodeId_t> &adjacent)
{
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(FINDER_GET_ADJACENT);
    Wire.write((byte *)&node, sizeof(node));
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, sizeof(NodeId_t) * maxNeighbors + sizeof(size_t));
    while (Wire.available() < sizeof(size_t))
    {
    }
    size_t numNeighbors = 0;
    Wire.readBytes((byte *)&numNeighbors, sizeof(numNeighbors));

    NodeId_t neighbors[numNeighbors] = {0};
    Wire.readBytes((byte *)neighbors, sizeof(NodeId_t) * numNeighbors);

    adjacent.purge();
    for (int i = 0; i < numNeighbors; i++)
    {
        adjacent.pushBack(neighbors[i]);
    }
}

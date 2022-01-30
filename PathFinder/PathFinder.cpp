#include <Arduino.h>
#include <Wire.h>
#include "PathFinder.h"

void PathFinder::addNode(NodeId_t node, NodeId_t *neighbors, size_t numNeighbors)
{
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn add node "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_ADD_NODE);
            Wire.write((byte *)&node, sizeof(NodeId_t));
            Wire.write((byte *)&numNeighbors, sizeof(size_t));
            Wire.write((byte *)neighbors, sizeof(NodeId_t) * numNeighbors);
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req add node "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, 1);
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

    while (Wire.available() < 1)
    {
    }
}

NodeId_t PathFinder::getNextStep(NodeId_t src, NodeId_t dest)
{
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn next step "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_GET_NEXT_STEP);
            Wire.write((byte *)&src, sizeof(NodeId_t));
            Wire.write((byte *)&dest, sizeof(NodeId_t));
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req next step "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, sizeof(NodeId_t));
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

    while (Wire.available() < sizeof(NodeId_t))
    {
    }
    NodeId_t nextStep = 0;
    Wire.readBytes((byte *)&nextStep, sizeof(nextStep));
    return nextStep;
}

void PathFinder::startDiscovery()
{
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn start disco "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_START_DISCOVERY);
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req start disco "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, 1);
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

    while (Wire.available() < 1)
    {
    }
}

DiscoveryStats PathFinder::getDiscoveryStats()
{
    size_t statSize = sizeof(bool) + sizeof(size_t) * 2;
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn disco stats "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_GET_DISCOVERY_STATS);
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req disco stats "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, statSize);
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

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
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn nxt neigh "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_GET_NEIGHBOR_REQUEST);
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req nxt neigh "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, sizeof(NodeId_t));
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

    while (Wire.available() < sizeof(NodeId_t))
    {
    }
    NodeId_t neighborRequest = 0;
    Wire.readBytes((byte *)&neighborRequest, sizeof(neighborRequest));
    return neighborRequest;
}

void PathFinder::resetIterator(NodeId_t start)
{
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn rst iter "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_ITERATOR_RESET);
            Wire.write((byte *)&start, sizeof(start));
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req rst iter "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, 1);
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

    while (Wire.available() < 1)
    {
    }
}

void clearIterator()
{
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn clear iter "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_ITERATOR_CLEAR);
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req clear iter "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, 1);
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

    while (Wire.available() < 1)
    {
    }
}

NodeId_t PathFinder::getIteratorNext()
{
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn nxt iter "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_ITERATOR_NEXT);
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req nxt iter "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, sizeof(NodeId_t));
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

    while (Wire.available() < sizeof(NodeId_t))
    {
    }
    NodeId_t nodeId = 0;
    Wire.readBytes((byte *)&nodeId, sizeof(nodeId));
    return nodeId;
}

void PathFinder::clearTopology()
{
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn clear topo "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_CLEAR_TOPOLOGY);
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req clear topo "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, 1);
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

    while (Wire.available() < 1)
    {
    }
}

void PathFinder::getAdjacent(NodeId_t node, Set<NodeId_t> &adjacent)
{
    int retries = 0;
    do
    {
        do
        {
            retries++;
            Serial.print(F("trn get adj "));
            Serial.println(retries);
            Wire.clearWireTimeoutFlag();
            Wire.beginTransmission(FINDER_I2C_ADDR);
            Wire.write(FINDER_GET_ADJACENT);
            Wire.write((byte *)&node, sizeof(node));
            Wire.endTransmission();
            if (Wire.getWireTimeoutFlag())
            {
                delay(1000);
            }
        } while (Wire.getWireTimeoutFlag());

        retries = 0;

        retries++;
        Serial.print(F("req get adj "));
        Serial.println(retries);
        Wire.clearWireTimeoutFlag();
        Wire.requestFrom(FINDER_I2C_ADDR, sizeof(NodeId_t) * maxNeighbors + sizeof(size_t));
        if (Wire.getWireTimeoutFlag())
        {
            delay(1000);
        }
    } while (Wire.getWireTimeoutFlag());

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

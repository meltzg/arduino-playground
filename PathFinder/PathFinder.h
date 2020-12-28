#ifndef _PATH_FINDER_H_
#define _PATH_FINDER_H_

#include "CommonMessaging.h"

#define FINDER_I2C_ADDR 0x01

// Pathfinder commands
#define FINDER_ADD_NODE 0x01
#define FINDER_GET_NEXT_STEP 0x02
#define FINDER_START_DISCOVERY 0x03
#define FINDER_GET_DISCOVERY_STATS 0x04
#define FINDER_GET_NEIGHBOR_REQUEST 0x05
#define FINDER_ITERATOR_RESET 0x06
#define FINDER_ITERATOR_NEXT 0x07
#define FINDER_CLEAR_TOPOLOGY 0x08

struct DiscoveryStats {
    const bool discoveryDone;
    const size_t numNodes;
    const size_t numEdges;

    DiscoveryStats(bool discoveryDone, size_t numNodes, size_t numEdges) : discoveryDone(discoveryDone), numNodes(numNodes), numEdges(numEdges) {}
};

class PathFinder
{
public:
    void addNode(NodeId_t node, NodeId_t *neighbors, size_t numNeighbors);
    NodeId_t getNextStep(NodeId_t src, NodeId_t dest);
    void startDicovery(NodeId_t node);
    DiscoveryStats getDiscoveryStats();
    NodeId_t getNextNeighborRequest();
    void resetIterator(NodeId_t start);
    NodeId_t getNextIterator();
    void clearTopology();
};

#endif _PATH_FINDER_H_
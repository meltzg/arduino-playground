#ifndef _PATH_FINDER_H_
#define _PATH_FINDER_H_

#include "CommonMessaging.h"
#include "DataStructures.h"

#define FINDER_I2C_ADDR 0x01

// Pathfinder commands
#define FINDER_ADD_NODE 0x01
#define FINDER_GET_NEXT_STEP 0x02
#define FINDER_START_DISCOVERY 0x03
#define FINDER_GET_DISCOVERY_STATS 0x04
#define FINDER_GET_NEIGHBOR_REQUEST 0x05
#define FINDER_ITERATOR_RESET 0x06
#define FINDER_ITERATOR_CLEAR 0x07
#define FINDER_ITERATOR_NEXT 0x08
#define FINDER_CLEAR_TOPOLOGY 0x09
#define FINDER_GET_ADJACENT 0x0A

struct DiscoveryStats
{
    const bool discoveryDone;
    const size_t numNodes;
    const size_t numEdges;

    DiscoveryStats(bool discoveryDone, size_t numNodes, size_t numEdges) : discoveryDone(discoveryDone), numNodes(numNodes), numEdges(numEdges) {}
};

class PathFinder
{
private:
    int maxNeighbors;

public:
    PathFinder(int maxNeighbors) : maxNeighbors(maxNeighbors) {}

    void addNode(NodeId_t node, NodeId_t *neighbors, size_t numNeighbors);
    NodeId_t getNextStep(NodeId_t src, NodeId_t dest);
    void startDiscovery();
    DiscoveryStats getDiscoveryStats();
    NodeId_t getNextNeighborRequest();
    void resetIterator(NodeId_t start);
    void clearIterator();
    NodeId_t getIteratorNext();
    void clearTopology();
    void getAdjacent(NodeId_t node, Set<NodeId_t> &adjacent);
};

#endif // _PATH_FINDER_H_

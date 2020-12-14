#include <iostream>
#include "structures.h"

using std::cout;
using std::endl;


int main(int argc, char **argv) {
  Graph<int> g;
  Map<int, int> pred;
  Map<int, int> dist;
  g.addEdge(0, 1);
  g.addEdge(0, 3);
  g.addEdge(1, 2);
  g.addEdge(3, 4);
  g.addEdge(3, 7);
  g.addEdge(4, 5);
  g.addEdge(4, 6);
  g.addEdge(4, 7);
  g.addEdge(5, 6);
  g.addEdge(6, 7);
  g.addEdge(12, 11);
  cout << "connected: " << g.bfs(0, 7, pred, dist) << endl;
  pred.purge();
  dist.purge();
  cout << "connected: " << g.bfs(1, 12, pred, dist) << endl;

  GraphIterator<int> giter(g, 3);
  while(giter.hasNext()) {
    cout << "node: " << giter.next() << endl;
  }
}
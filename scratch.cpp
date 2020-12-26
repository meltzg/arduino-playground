#include <iostream>
#include "DataStructures/DataStructures.h"

using std::cout;
using std::endl;

int main(int argc, char **argv)
{
  Graph<int> g;
  Map<int, int> pred;
  g.addEdge(0, 1);
  g.addEdge(0, 4);
  g.addEdge(0, 5);
  g.addEdge(1, 2);
  g.addEdge(1, 5);
  g.addEdge(1, 6);
  g.addEdge(2, 3);
  g.addEdge(2, 6);
  g.addEdge(2, 7);
  g.addEdge(3, 7);
  g.addEdge(3, 8);
  g.addEdge(4, 5);
  g.addEdge(4, 9);
  g.addEdge(4, 10);
  g.addEdge(5, 6);
  g.addEdge(5, 10);
  g.addEdge(5, 11);
  g.addEdge(6, 7);
  g.addEdge(6, 11);
  g.addEdge(6, 12);
  g.addEdge(7, 8);
  g.addEdge(7, 12);
  g.addEdge(7, 13);
  g.addEdge(8, 13);
  g.addEdge(8, 14);
  g.addEdge(9, 10);
  g.addEdge(9, 15);
  g.addEdge(9, 16);
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
  g.addEdge(14, 20);
  g.addEdge(14, 21);
  g.addEdge(15, 16);
  g.addEdge(15, 22);
  g.addEdge(15, 23);
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
  g.addEdge(21, 28);
  g.addEdge(21, 29);
  g.addEdge(22, 23);
  g.addEdge(22, 30);
  g.addEdge(23, 24);
  g.addEdge(23, 31);
  g.addEdge(23, 30);
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
  g.addEdge(29, 36);
  g.addEdge(30, 31);
  g.addEdge(30, 37);
  g.addEdge(31, 32);
  g.addEdge(31, 38);
  g.addEdge(31, 37);
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
  g.addEdge(36, 42);
  g.addEdge(37, 38);
  g.addEdge(37, 43);
  g.addEdge(38, 39);
  g.addEdge(38, 44);
  g.addEdge(38, 43);
  g.addEdge(39, 40);
  g.addEdge(39, 45);
  g.addEdge(39, 44);
  g.addEdge(40, 41);
  g.addEdge(40, 46);
  g.addEdge(40, 45);
  g.addEdge(41, 42);
  g.addEdge(41, 47);
  g.addEdge(41, 46);
  g.addEdge(42, 47);
  g.addEdge(43, 44);
  g.addEdge(43, 48);
  g.addEdge(44, 45);
  g.addEdge(44, 49);
  g.addEdge(44, 48);
  g.addEdge(45, 46);
  g.addEdge(45, 50);
  g.addEdge(45, 49);
  g.addEdge(46, 47);
  g.addEdge(46, 51);
  g.addEdge(46, 50);
  g.addEdge(47, 51);
  g.addEdge(48, 49);
  g.addEdge(49, 50);
  g.addEdge(50, 51);

  cout << "num edges " << g.numEdges() << " num nodes " << g.numNodes() << endl;

  cout << "connected: " << g.bfs(0, 7, pred) << endl;
  pred.purge();
  cout << "connected: " << g.bfs(1, 12, pred) << endl;

  GraphIterator<int> giter(g, 3);
  while (giter.hasNext())
  {
    cout << "node: " << giter.next() << endl;
  }

  for (int i = 0; i < 52; i++)
  {
    for (int j = 0; j < 52; j++)
    {
      if (i == j) {
        continue;
      }
      LinkedList<int> path;
      LinkedList<int> revPath;
      g.getShortestPath(i, j, path);
      g.getShortestPath(j, i, revPath);
      if (path.isEmpty())
      {
        cout << "Nodes " << i << ", " << j << endl;
        cout << "path error" << endl;
      }
      if (revPath.isEmpty())
      {
        cout << "Nodes " << i << ", " << j << endl;
        cout << "revPath error" << endl;
      }
    }
  }
}

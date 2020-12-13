#include <iostream>
#include "structures.h"

using std::cout;
using std::endl;


int main(int argc, char **argv) {
  Graph<int> g;
  g.addEdge(1, 2);
  g.addEdge(2, 1);
  cout << "edges: " << g.edges.front->val.left << "<->" << g.edges.front->val.right << endl;
}
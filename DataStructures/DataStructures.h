#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#ifdef __AVR__
#include <Arduino.h>
#else
#include <cstddef>
#endif

template <typename T>
struct LinkedNode
{
  T val;
  LinkedNode *next;

  LinkedNode(T val) : val(val), next(NULL) {}
};

template <typename T>
struct LinkedList
{
  LinkedNode<T> *front, *back;
  size_t count = 0;

  LinkedList()
  {
    front = back = NULL;
  }
  ~LinkedList()
  {
    purge();
  }
  LinkedList(LinkedList<T> const &other)
  {
    front = back = NULL;
    for (auto iter = other.front; iter != NULL; iter = iter->next)
    {
      pushBack(iter->val);
    }
  }

  virtual void pushBack(T val)
  {
    LinkedNode<T> *tmp = new LinkedNode<T>(val);
    if (isEmpty())
    {
      front = back = tmp;
    }
    else
    {
      back->next = tmp;
      back = tmp;
    }
    count++;
  }

  virtual void pushFront(T val)
  {
    LinkedNode<T> *tmp = new LinkedNode<T>(val);
    if (isEmpty())
    {
      front = back = tmp;
    }
    else
    {
      tmp->next = front;
      front = tmp;
    }
    count++;
  }

  T popFront()
  {
    LinkedNode<T> *tmp = front;
    T val = tmp->val;
    front = front->next;
    delete tmp;
    count--;
    if (count == 0)
    {
      back = NULL;
    }
    return val;
  }

  T peekFront()
  {
    return front->val;
  }

  T peekBack()
  {
    return back->val;
  }

  bool isEmpty()
  {
    return back == NULL;
  }

  bool contains(T val)
  {
    LinkedNode<T> *node = front;
    while (node != NULL)
    {
      if (node->val == val)
      {
        return true;
      }
      node = node->next;
    }
    return false;
  }

  void purge()
  {
    while (front != NULL)
    {
      popFront();
    }
  }
};

template <typename T>
class ListIterator
{
private:
  LinkedNode<T> *current;

public:
  ListIterator(const LinkedList<T> &list)
  {
    current = list.front;
  }

  bool hasNext()
  {
    return current != NULL;
  }

  T next()
  {
    T val = current->val;
    current = current->next;
    return val;
  }
};

template <typename T>
struct Set : public LinkedList<T>
{
  void pushBack(T val)
  {
    if (this->contains(val))
    {
      return;
    }
    LinkedList<T>::pushBack(val);
  }

  void pushFront(T val)
  {
    if (this->contains(val))
    {
      return;
    }
    LinkedList<T>::pushFront(val);
  }
};

template <typename K, typename V>
struct Pair
{
  K left;
  V right;

  Pair(K left, V right) : left(left), right(right) {}
};

template <typename K, typename V>
struct Map
{
  LinkedList<Pair<K, V>> values;

  ~Map()
  {
    purge();
  }

  void put(K key, V value)
  {
    for (auto iter = values.front; iter != NULL; iter = iter->next)
    {
      if (iter->val.left == key)
      {
        iter->val.right = value;
        return;
      }
    }
    values.pushBack(Pair<K, V>(key, value));
  }

  V *get(K key)
  {
    for (auto iter = values.front; iter != NULL; iter = iter->next)
    {
      if (iter->val.left == key)
      {
        return &(iter->val.right);
      }
    }
  }

  bool containsKey(K key)
  {
    for (auto iter = values.front; iter != NULL; iter = iter->next)
    {
      if (iter->val.left == key)
      {
        return true;
      }
    }

    return false;
  }

  void purge()
  {
    values.purge();
  }
};

template <typename T>
struct GraphEdge : public Pair<T, T>
{
  using Pair<T, T>::Pair;

  bool operator==(const GraphEdge<T> &other)
  {
    return (this->left == other.left && this->right == other.right) or (this->left == other.right && this->right == other.left);
  }
};

template <typename T>
class Graph
{
private:
  Set<GraphEdge<T>> edges;

public:
  void addEdge(T src, T dest)
  {
    edges.pushBack(GraphEdge<T>(src, dest));
  }

  const Set<GraphEdge<T>> *getEdges()
  {
    return &edges;
  }

  void purge()
  {
    edges.purge();
  }

  bool bfs(T src, T dest, Map<T, T> &pred)
  {
    Set<T> visited;
    LinkedList<T> queue;

    visited.pushBack(src);
    queue.pushBack(src);

    while (!queue.isEmpty())
    {
      T node = queue.popFront();
      Set<T> adjacent;
      getAdjacent(node, adjacent);
      for (ListIterator<T> iter(adjacent); iter.hasNext();)
      {
        T adjNode = iter.next();
        if (!visited.contains(adjNode))
        {
          visited.pushBack(adjNode);
          pred.put(adjNode, node);
          queue.pushBack(adjNode);

          if (adjNode == dest)
          {
            return true;
          }
        }
      }
    }

    return false;
  }

  void getShortestPath(T src, T dest, LinkedList<T> &path)
  {
    Map<T, T> pred;
    path.purge();

    if (bfs(src, dest, pred))
    {
      T crawl = dest;
      path.pushFront(crawl);
      while (pred.containsKey(crawl))
      {
        path.pushFront(*(pred.get(crawl)));
        crawl = *(pred.get(crawl));
      }
    }
  }

  void getAdjacent(T node, Set<T> &adjacent)
  {
    adjacent.purge();
    for (ListIterator<GraphEdge<T>> iter(edges); iter.hasNext();)
    {
      GraphEdge<T> edge = iter.next();
      if (edge.left == node)
      {
        adjacent.pushBack(edge.right);
      }
      else if (edge.right == node)
      {
        adjacent.pushBack(edge.left);
      }
    }
  }

  size_t numNodes()
  {
    Set<T> nodes;
    for (ListIterator<GraphEdge<T>> iter(edges); iter.hasNext();)
    {
      GraphEdge<T> edge = iter.next();
      nodes.pushBack(edge.left);
      nodes.pushBack(edge.right);
    }
    return nodes.count;
  }

  size_t numEdges()
  {
    return edges.count;
  }
};

template <typename T>
struct GraphIterator
{
  Graph<T> g;
  Set<T> visited;
  LinkedList<T> queue;

  GraphIterator(const Graph<T> &g, T start) : g(g)
  {
    queue.pushBack(start);
    visited.pushBack(start);
  }

  bool hasNext()
  {
    return !queue.isEmpty();
  }

  T next()
  {
    T next = queue.popFront();
    Set<T> adjacent;
    g.getAdjacent(next, adjacent);
    for (ListIterator<T> iter(adjacent); iter.hasNext();)
    {
      T adjNode = iter.next();
      if (!visited.contains(adjNode))
      {
        visited.pushBack(adjNode);
        queue.pushBack(adjNode);
      }
    }
    return next;
  }
};

#endif // _STRUCTURES_H_

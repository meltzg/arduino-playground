#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

template <typename T> struct LinkedNode {
  T val;
  LinkedNode *next;

  LinkedNode(T val) : val(val), next(NULL) {}
};

template <typename T> struct LinkedList {
  LinkedNode<T> *front, *back;
  size_t count = 0;

  LinkedList() {
    front = back = NULL;
  }
  ~LinkedList() {
    purge();
  }
  LinkedList(LinkedList<T> const &other) {
    front = back = NULL;
    for (auto iter = other.front; iter != NULL; iter = iter->next) {
      pushBack(iter->val);
    }
  }

  virtual void pushBack(T val) {
    LinkedNode<T> *tmp = new LinkedNode<T>(val);
    if (isEmpty()) {
      front = back = tmp;
    } else {
      back->next = tmp;
      back = tmp;
    }
    count++;
  }

  virtual void pushFront(T val) {
    LinkedNode<T> *tmp = new LinkedNode<T>(val);
    if (isEmpty()) {
      front = back = tmp;
    } else {
      tmp->next = front;
      front = tmp;
    }
    count++;
  }

  T popFront() {
    LinkedNode<T> *tmp = front;
    T val = tmp->val;
    front = front->next;
    delete tmp;
    count--;
    if (count == 0) {
      back = NULL;
    }
    return val;
  }

  T peekFront() {
    return front->val;
  }

  T peekBack() {
    return back->val;
  }

  bool isEmpty() {
    return back == NULL;
  }

  bool contains(T val) {
    LinkedNode<T> *node = front;
    while (node != NULL) {
      if (node->val == val) {
        return true;
      }
      node = node->next;
    }
    return false;
  }

  void purge() {
    while (front != NULL) {
      popFront();
    }
  }
};

template <typename T> struct Set : public LinkedList<T> {
  void pushBack(T val) {
    if (this->contains(val)) {
      return;
    }
    LinkedList<T>::pushBack(val);
  }

  void pushFront(T val) {
    if (this->contains(val)) {
      return;
    }
    LinkedList<T>::pushFront(val);
  }
};

template <typename K, typename V> struct Pair {
  K left;
  V right;

  Pair(K left, V right) : left(left), right(right) {}
};

template <typename K, typename V> struct Map {
  LinkedList<Pair<K, V>> values;

  ~Map() {
    purge();
  }

  void put(K key, V value) {
    for (auto iter = values.front; iter != NULL; iter = iter->next) {
      if (iter->val.left == key) {
        iter->val.right = value;
        return;
      }
    }
    values.pushBack(Pair<K, V>(key, value));
  }

  V *get(K key) {
    for (auto iter = values.front; iter != NULL; iter = iter->next) {
      if (iter->val.left == key) {
        return &(iter->val.right);
      }
    }
  }

  bool containsKey(K key) {
    for (auto iter = values.front; iter != NULL; iter = iter->next) {
      if (iter->val.left == key) {
        return true;
      }
    }

    return false;
  }

  void purge() {
    values.purge();
  }
};

template <typename T> struct Graph {
  Map<T, Set<T>> adj;

  void addEdge(T src, T dest) {
    if (!adj.containsKey(src)) {
      adj.put(src, Set<T>());
    }
    if (!adj.containsKey(dest)) {
      adj.put(dest, Set<T>());
    }

    adj.get(src)->pushBack(dest);
    adj.get(dest)->pushBack(src);
  }

  void purge() {
    adj.purge();
  }

  bool bfs(T src, T dest, Map<T, T> &pred, Map<T, int> &dist) {
    Set<T> visited;
    LinkedList<T> queue;

    visited.pushBack(src);
    dist.put(src, 0);
    queue.pushBack(src);

    while (!queue.isEmpty()) {
      T node = queue.popFront();
      for (auto iter = adj.get(node)->front; iter != NULL; iter = iter->next) {
        if (!visited.contains(iter->val)) {
          visited.pushBack(iter->val);
          dist.put(iter->val, *(dist.get(node)) + 1);
          pred.put(iter->val, node);
          queue.pushBack(iter->val);

          if (iter->val == dest) {
            return true;
          }
        }
      }
    }

    return false;
  }

  void getShortestPath(T src, T dest, LinkedList<T> &path) {
    Map<T, T> pred;
    Map<T, int> dist;

    if (bfs(src, dest, pred, dist)) {
      T crawl = dest;
      path.pushFront(crawl);
      while (pred.containsKey(crawl)) {
        path.pushFront(*(pred.get(crawl)));
        crawl = *(pred.get(crawl));
      }
    }
  }
};

template <typename T> struct GraphIterator {
  Graph<T> g;
  Set<T> visited;
  LinkedList<T> queue;

  GraphIterator(Graph<T> g, T start) : g(g) {
    queue.pushBack(start);
    visited.pushBack(start);
  }

  bool hasNext() {
    return !queue.isEmpty();
  }

  T next() {
    T next = queue.popFront();
    for (auto iter = g.adj.get(next)->front; iter != NULL; iter = iter->next) {
      if (!visited.contains(iter->val)) {
        visited.pushBack(iter->val);
        queue.pushBack(iter->val);
      }
    }
    return next;
  }
};

#endif // _STRUCTURES_H_

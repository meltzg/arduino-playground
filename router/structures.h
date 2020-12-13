#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#include <cstddef>

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

  T popFront() {
    LinkedNode<T> *tmp = front;
    T val = tmp->val;
    front = front->next;
    delete tmp;
    count--;
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
  virtual void pushBack(T val) {
    if (this->contains(val)) {
      return;
    }
    LinkedList<T>::pushBack(val);
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

  V& get(K key) {
    for (auto iter = values.front; iter != NULL; iter = iter->next) {
      if (iter->val.left == key) {
        return iter->val.right;
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

    adj.get(src).pushBack(dest);
    adj.get(dest).pushBack(src);
  }

  void purge() {
    adj.purge();
  }

  LinkedList<T> shortestPath(T src, T dest) {

  }
};

#endif // _STRUCTURES_H_

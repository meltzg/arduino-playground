template <typename T> struct LinkedNode {
  T val;
  LinkedNode *next;

  LinkedNode(T val) : val(val) {}
};

template <typename T> struct LinkedList {
  LinkedNode<T> *front, *back;

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
      return;
    }
    back->next = tmp;
    back = tmp;
  }

  T popFront() {
    LinkedNode<T> *tmp = front;
    T val = tmp->val;
    front = front->next;
    delete tmp;
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
    if (contains(val)) {
      return;
    }
    LinkedList<T>::pushBack(val);
  }  
};

template <typename T> struct GraphEdge {
  T src;
  T dest;

  GraphEdge(T src, T dest) : src(src), dest(dest) {}

  bool operator==(const GraphEdge<T> &rhs) {
    return (src == rhs.src && dest == rhs.dest) || (src == rhs.dest && dest == rhs.src);
  }
};

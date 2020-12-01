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
    while (front != NULL) {
      popFront();
    }
  }

  void pushBack(T val) {
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
};

#ifndef _DATA_STRUCTURES_H_
#define _DATA_STRUCTURES_H_

#ifdef __AVR__
#include <EEPROM.h>
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
        return count == 0;
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

    virtual V *get(K key)
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

template <typename K, typename V>
struct DefaultMap : public Map<K, V>
{
    V defaultValue;

    DefaultMap(V defaultValue) : defaultValue(defaultValue) {}

    V *get(K key)
    {
        if (!this->containsKey(key))
        {
            this->put(key, defaultValue);
        }
        return Map<K, V>::get(key);
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
    bool useEeprom;
    int eepromOffset;
    int eepromMax;

public:
    Graph(bool useEeprom = false, int eepromOffset = 0, int eepromMax = -1, bool initPurge = true) : useEeprom(useEeprom), eepromOffset(eepromOffset), eepromMax(eepromMax)
    {
        if (initPurge)
        {
            edges.purge();
        }
    }

    bool addEdge(T src, T dest)
    {
#ifdef __AVR__
        if (useEeprom)
        {
            int sizeOfEdge = 2 * sizeof(T);

            size_t eepromCount = 0;
            EEPROM.get(eepromOffset, eepromCount);
            for (int i = 0; i < eepromCount; i++)
            {
                int eepromLocation = eepromOffset + (i * sizeOfEdge) + sizeof(eepromCount);
                T existingSrc = 0, existingDest = 0;

                for (int j = 0; j < sizeof(T); j++)
                {
                    existingSrc += EEPROM.read(eepromLocation + j) << (8 * j);
                }
                for (int j = 0; j < sizeof(T); j++)
                {
                    existingDest += EEPROM.read(eepromLocation + sizeof(T) + j) << (8 * j);
                }
                if (src == existingSrc && dest == existingDest || dest == existingSrc && src == existingDest)
                {
                    return true;
                }
            }

            int maxLoc = eepromMax >= 0 ? eepromMax : EEPROM.length();
            if (eepromOffset + sizeof(eepromCount) + (eepromCount * sizeOfEdge) > maxLoc)
            {
                return false;
            }
            for (int i = 0; i < sizeof(T); i++)
            {
                EEPROM.update(eepromOffset + sizeof(eepromCount) + (eepromCount * sizeOfEdge) + i, (src >> (8 * i)));
            }
            for (int i = 0; i < sizeof(T); i++)
            {
                EEPROM.update(eepromOffset + sizeof(eepromCount) + (eepromCount * sizeOfEdge) + sizeof(T) + i, (dest >> (8 * i)));
            }
            eepromCount++;
            EEPROM.put(eepromOffset, eepromCount);
            return true;
        }
#endif
        edges.pushBack(GraphEdge<T>(src, dest));
        return true;
    }

    void purge()
    {
        size_t eepromCount = 0;
        EEPROM.put(eepromOffset, eepromCount);
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

#ifdef __AVR__
        if (useEeprom)
        {
            int sizeOfEdge = 2 * sizeof(T);
            size_t eepromCount = 0;
            EEPROM.get(eepromOffset, eepromCount);
            for (int i = 0; i < eepromCount; i++)
            {
                int eepromLocation = eepromOffset + sizeof(eepromCount) + (i * sizeOfEdge);
                T src = 0, dest = 0;

                for (int j = 0; j < sizeof(T); j++)
                {
                    src += EEPROM.read(eepromLocation + j) << (8 * j);
                }
                for (int j = 0; j < sizeof(T); j++)
                {
                    dest += EEPROM.read(eepromLocation + sizeof(T) + j) << (8 * j);
                }
                if (src == node)
                {
                    adjacent.pushBack(dest);
                }
                else if (dest == node)
                {
                    adjacent.pushBack(src);
                }
            }
            return;
        }
#endif
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

    size_t numAdjacent(T node)
    {
        size_t size = 0;

#ifdef __AVR__
        if (useEeprom)
        {
            int sizeOfEdge = 2 * sizeof(T);
            size_t eepromCount = 0;
            EEPROM.get(eepromOffset, eepromCount);
            for (int i = 0; i < eepromCount; i++)
            {
                int eepromLocation = eepromOffset + sizeof(eepromCount) + (i * sizeOfEdge);
                T src = 0, dest = 0;

                for (int j = 0; j < sizeof(T); j++)
                {
                    src += EEPROM.read(eepromLocation + j) << (8 * j);
                }
                for (int j = 0; j < sizeof(T); j++)
                {
                    dest += EEPROM.read(eepromLocation + sizeof(T) + j) << (8 * j);
                }
                if (src == node || dest == node)
                {
                    size++;
                }
            }
            return size;
        }
#endif
        for (ListIterator<GraphEdge<T>> iter(edges); iter.hasNext();)
        {
            GraphEdge<T> edge = iter.next();
            if (edge.left == node || edge.right == node)
            {
                size++;
            }
        }

        return size;
    }

    size_t numNodes()
    {
        Set<T> nodes;
#ifdef __AVR__
        if (useEeprom)
        {
            int sizeOfEdge = 2 * sizeof(T);
            size_t eepromCount = 0;
            EEPROM.get(eepromOffset, eepromCount);
            for (int i = 0; i < eepromCount; i++)
            {
                int eepromLocation = eepromOffset + sizeof(eepromCount) + (i * sizeOfEdge);
                T src = 0, dest = 0;

                for (int j = 0; j < sizeof(T); j++)
                {
                    src += EEPROM.read(eepromLocation + j) << (8 * j);
                }
                for (int j = 0; j < sizeof(T); j++)
                {
                    dest += EEPROM.read(eepromLocation + sizeof(T) + j) << (8 * j);
                }
                nodes.pushBack(src);
                nodes.pushBack(dest);
            }
        }
#endif
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
#ifdef __AVR__
        if (useEeprom)
        {
            size_t eepromCount = 0;
            EEPROM.get(eepromOffset, eepromCount);
            return eepromCount;
        }
#endif
        return edges.count;
    }
};

template <typename T>
struct GraphIterator
{
    Graph<T> g;
    Set<T> visited;
    LinkedList<T> queue;

    GraphIterator() {}

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

    void clear()
    {
        visited.purge();
        queue.purge();
    }

    void reset(const Graph<T> &g, T start)
    {
        clear();
        this->g = g;
        queue.pushBack(start);
        visited.pushBack(start);
    }
};

#endif // _DATA_STRUCTURES_H_

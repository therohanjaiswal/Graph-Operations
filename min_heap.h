#include <bits/stdc++.h>
using namespace std;

class HeapNode
{
    int v;
    int dist;

    HeapNode(int vertex, int distance)
    {
        v = vertex;
        dist = distance;
    }
    friend class MinHeap;
    friend class Graph;
};

class MinHeap
{
private:
    int size;     // Number of heap nodes present currently
    int capacity; // Capacity of min heap
    int *pos;     // This is needed for decreaseKey()
    HeapNode **array;

public:
    MinHeap(int cap)
    {
        pos = (int *)malloc(cap * sizeof(int));
        size = 0;
        capacity = cap;
        array = (HeapNode **)malloc(cap * sizeof(HeapNode));
    }

    HeapNode *newMinHeapNode(int, int);
    void swapMinHeapNode(int, int);
    int isEmpty();
    HeapNode *extractMin();
    void decreaseKey(int, int);
    bool isInMinHeap(int);
    void minHeapify(int);

    friend class Graph;
};

// new Min Heap Node
HeapNode *MinHeap::newMinHeapNode(int v, int dist)
{
    HeapNode *minHeapNode = new HeapNode(v, dist);
    return minHeapNode;
}

// A utility function to swap two nodes of min heap.
// Needed for min heapify.
void MinHeap::swapMinHeapNode(int a, int b)
{
    HeapNode *t = array[a];
    array[a] = array[b];
    array[b] = t;
}

// A standard function to heapify at given idx
// This function also updates position of nodes when they are swapped.
// Position is needed for decreaseKey()
void MinHeap::minHeapify(int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < size && array[left]->dist < array[smallest]->dist)
        smallest = left;

    if (right < size && array[right]->dist < array[smallest]->dist)
        smallest = right;

    if (smallest != idx)
    {
        // The nodes to be swapped in min heap
        HeapNode *smallestNode = array[smallest];
        HeapNode *idxNode = array[idx];

        // Swap positions
        pos[smallestNode->v] = idx;
        pos[idxNode->v] = smallest;

        // Swap nodes
        swapMinHeapNode(smallest, idx);

        minHeapify(smallest);
    }
}

// A utility function to check if the given minHeap is empty or not
int MinHeap::isEmpty()
{
    return size == 0;
}

// Standard function to extract minimum node from heap
HeapNode *MinHeap::extractMin()
{
    if (isEmpty())
        return NULL;

    // Store the root node
    HeapNode *root = array[0];

    // Replace root node with last node
    HeapNode *lastNode = array[size - 1];
    array[0] = lastNode;

    // Update position of last node
    pos[root->v] = size - 1;
    pos[lastNode->v] = 0;

    // Reduce heap size and heapify root
    --size;
    minHeapify(0);
    return root;
}

// Function to decreasy dist value of a given vertex v. This function
// uses pos[] of min heap to get the current index of node in min heap
void MinHeap::decreaseKey(int v, int dist)
{
    // Get the index of v in  heap array
    int i = pos[v];

    // Get the node and update its dist value
    array[i]->dist = dist;

    // Travel up while the complete tree is not hepified.
    // This is a O(Logn) loop
    while (i && array[i]->dist < array[(i - 1) / 2]->dist)
    {
        // Swap this node with its parent
        pos[array[i]->v] = (i - 1) / 2;
        pos[array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(i, (i - 1) / 2);
        i = (i - 1) / 2; // move to parent index
    }
}

// A utility function to check if a given vertex 'v' is in min heap or not
bool MinHeap::isInMinHeap(int v)
{
    return pos[v] < size ? true : false;
}
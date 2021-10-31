#include <bits/stdc++.h>
using namespace std;

/* Heap Node */
class HeapNode
{
private:
    int v;    // total number of vertices
    int dist; // distance from source vertex

public:
    // constructor
    HeapNode(int vertex, int distance)
    {
        v = vertex;
        dist = distance;
    }

    friend class MinHeap;
    friend class Graph;
};

/* MinHeap class represents MinHeeap(Priority Queue) Structure*/
class MinHeap
{
private:
    int size;     // Total Number of nodes present in min heap currently
    int capacity; // Capacity of min heap
    // This is needed for decreaseKey()
    int *pos;
    HeapNode **array;

public:
    // constructor
    MinHeap(int cap)
    {
        pos = (int *)malloc(cap * sizeof(int));
        size = 0;
        capacity = cap;
        array = (HeapNode **)malloc(cap * sizeof(HeapNode));
    }

    HeapNode *create_minheap_node(int, int);
    void swap_minheap_node(int, int);
    int is_empty();
    HeapNode *extract_min();
    void decrease_key(int, int);
    bool is_in_min_heap(int);
    void min_heapify(int);

    friend class Graph;
};

/* This function creates new min heap node */
HeapNode *MinHeap::create_minheap_node(int v, int dist)
{
    HeapNode *minHeapNode = new HeapNode(v, dist);
    return minHeapNode;
}

/* Utility function to swap two nodes of min heap */
void MinHeap::swap_minheap_node(int a, int b)
{
    HeapNode *t = array[a];
    array[a] = array[b];
    array[b] = t;
}

/* This function heapify at given index and also updates 
 position of nodes when swapped */
void MinHeap::min_heapify(int index)
{
    int smallest = index;
    int left_child = 2 * index + 1;
    int right_child = 2 * index + 2;

    // if left child of idx is small
    if (left_child < size && array[left_child]->dist < array[smallest]->dist)
        smallest = left_child;

    // if right child of idx is small
    if (right_child < size && array[right_child]->dist < array[smallest]->dist)
        smallest = right_child;

    if (smallest != index)
    {
        // The nodes to be swapped in min heap
        HeapNode *smallest_node = array[smallest];
        HeapNode *index_node = array[index];

        // Swap positions
        pos[smallest_node->v] = index;
        pos[index_node->v] = smallest;

        // Swap nodes
        swap_minheap_node(smallest, index);

        min_heapify(smallest);
    }
}

/* This function extracts minimum node from min heap */
HeapNode *MinHeap::extract_min()
{
    // if min heap is empty
    if (size == 0)
        return NULL;

    HeapNode *root = array[0]; // pointer pointing to root

    // Replace root node with last node
    HeapNode *lastNode = array[size - 1];
    array[0] = lastNode;

    // Update position of last node
    pos[root->v] = size - 1;
    pos[lastNode->v] = 0;

    // Reduce heap size and heapify root
    --size;
    min_heapify(0);
    return root;
}

/* This function decrease distance value of vertex v */
void MinHeap::decrease_key(int v, int dist)
{
    int i = pos[v];        // store vertex v index
    array[i]->dist = dist; // update its distance value

    // Iterate until min heap is heapified.
    while (i && array[i]->dist < array[(i - 1) / 2]->dist)
    {
        // Swap this node with its parent
        pos[array[i]->v] = (i - 1) / 2;
        pos[array[(i - 1) / 2]->v] = i;
        swap_minheap_node(i, (i - 1) / 2);
        i = (i - 1) / 2; // move to parent index
    }
}

/* Utility function to check if a given vertex 'v' 
    is present in min heap or not */
bool MinHeap::is_in_min_heap(int v)
{
    return pos[v] < size ? true : false;
}
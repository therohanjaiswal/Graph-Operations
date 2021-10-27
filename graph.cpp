#include <bits/stdc++.h>
#include "min_heap.h"
#include <stdlib.h>

using namespace std;

int timer = 1;

class Graph
{
private:
    vector<pair<int, int>> *adj = nullptr;
    int V;
    int e;
    int min_distance(int *, bool *);
    void traverse_dfs(bool *, int *, int *, string &, string &, int);

public:
    Graph(int vertices)
    {
        V = vertices;
        adj = new vector<pair<int, int>>[V];
    }

    void add_edge(int, int, int);
    void print_graph();
    void dfs();
    void dijkstra(int);
};

// The main function that calculates distances of shortest paths from src to all
// vertices. It is a O(ELogV) function
void Graph::dijkstra(int src)
{
    // dist values used to pick minimum weight edge in cut
    int dist[V];

    // minHeap represents set E
    MinHeap *minHeap = new MinHeap(V);

    // Initialize min heap with all vertices. dist value of all vertices
    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT_MAX;
        minHeap->array[v] = minHeap->newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    // Make dist value of src vertex as 0 so that it is extracted first
    minHeap->array[src] = minHeap->newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    minHeap->decreaseKey(src, dist[src]);

    // Initially size of min heap is equal to V
    minHeap->size = V;

    // In the followin loop, min heap contains all nodes whose shortest distance
    // is not yet finalized.
    while (!minHeap->isEmpty())
    {
        // Extract the vertex with minimum distance value
        HeapNode *minHeapNode = minHeap->extractMin();

        // Store the extracted vertex number
        int u = minHeapNode->v;

        // Traverse through all adjacent vertices of u (the extracted
        // vertex) and update their distance values
        for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
        {
            int v = it->first;
            if (minHeap->isInMinHeap(v) && dist[u] != INT_MAX && it->second + dist[u] < dist[v])
            {
                dist[v] = dist[u] + it->second;
                minHeap->decreaseKey(v, dist[v]);
            }
        }
    }

    // print the calculated shortest distances
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < V; ++i)
        printf("%d \t\t %d\n", i, dist[i]);
}

void Graph::dfs()
{
    ofstream stream;
    string str = "dfs.gv";
    stream.open(str);
    stream << "digraph g { \nranksep = \"equally\";\nrankdir = LR;\n";

    string node_string = "";
    string edge_string = "";
    ostringstream oss;
    timer = 1;

    bool *visited = new bool[V];
    int *start_time = new int[V];
    int *end_time = new int[V];

    for (int i = 0; i < V; i++)
    {
        visited[i] = false;
        start_time[i] = 0;
        end_time[i] = 0;
    }

    for (int i = 0; i < V; i++)
        if (*(visited + i) == false)
            traverse_dfs(visited, start_time, end_time, edge_string, node_string, i);

    stream << node_string;
    stream << edge_string;
    stream << "}";
    stream.close();
}

void Graph::traverse_dfs(bool *visited, int *start, int *end, string &edge_str, string &node_str, int i)
{
    ostringstream oss;
    ostringstream oss1;
    *(visited + i) = true;
    *(start + i) = timer++;
    oss1 << i << "[label = \"" << i << "(" << *(start + i) << ",";

    for (auto it = adj[i].begin(); it != adj[i].end(); ++it)
    {
        if (*(visited + it->first) == false)
        {
            oss << i << "->" << it->first << "[label=\" " << it->second << " \"];\n";
            edge_str.append(oss.str());
            oss.str("");
            oss.clear();
            traverse_dfs(visited, start, end, edge_str, node_str, it->first);
        }
        else
        {
            if (*(end + it->first) == 0)
            {
                // back
                oss << i << "->" << it->first << "[label=\" " << it->second << "\" color=\"green\"];\n";
                edge_str.append(oss.str());
            }
            else if (*(start + i) < *(start + it->first))
            { // forward
                oss << i << "->" << it->first << "[label=\" " << it->second << "\" color=\"red\"];\n";
                edge_str.append(oss.str());
            }
            else
            { // cross
                oss << i << "->" << it->first << "[label=\" " << it->second << "\" color=\"blue\"];\n";
                edge_str.append(oss.str());
            }
            oss.str("");
            oss.clear();
        }
    }
    *(end + i) = timer++;
    oss1 << *(end + i) << ")\"];\n";
    node_str.append(oss1.str());
    oss1.str("");
    oss1.clear();
}

void Graph::add_edge(int u, int v, int wt)
{
    adj[u].push_back(make_pair(v, wt));
}

// Print adjacency list representation ot graph
void Graph::print_graph()
{
    ofstream stream;
    string str = "graph.gv";
    stream.open(str);
    stream << "digraph g { \nranksep = \"equally\";\nrankdir = LR;\n";

    string rank_string = "";  // to store the details of every node
    string edges_string = ""; // to store the pointers details
    ostringstream oss;

    bool *visited = new bool[V];
    int *exp_time = new int[V];
    int time = 0;

    for (int i = 0; i < V; i++)
    {
        visited[i] = false;
        exp_time[i] = -1;
    }

    // Create a queue for BFS
    queue<int> queue; // queue to perform level order traversal

    // Mark the current node as visited and enqueue it
    for (int i = 0; i < V; i++)
    {
        if (*(visited + i) == false)
        {
            queue.push(i);
            while (!queue.empty())
            {
                int s = queue.front();
                *(visited + s) = true;
                if (*(exp_time + s) == -1)
                    *(exp_time + s) = time;
                ++time;
                queue.pop();

                // oss << "{rank = same; " << s << "; };\n";
                // rankString.append(oss.str());
                // oss.str("");
                // oss.clear();

                for (auto it = adj[s].begin(); it != adj[s].end(); ++it)
                {
                    if (*(visited + it->first) == false)
                    {
                        *(visited + it->first) = true;
                        if (*(exp_time + it->first) == -1)
                            *(exp_time + it->first) = time;
                        queue.push(it->first);
                        oss << s << "->" << it->first << "[label=\" " << it->second << " \"];\n";
                        edges_string.append(oss.str());
                    }
                    else
                    {
                        if (*(exp_time + s) == *(exp_time + it->first))
                        {
                            oss << s << "->" << it->first << "[label=\"" << it->second << "\" color=\"green\"];\n";
                            edges_string.append(oss.str());
                        }
                        else if (*(exp_time + s) > *(exp_time + it->first))
                        {
                            oss << s << "->" << it->first << "[label=\"" << it->second << "\" color=\"red\"];\n";
                            edges_string.append(oss.str());
                        }
                    }
                    oss.str("");
                    oss.clear();
                }
            }
        }
    }

    stream << edges_string;
    stream << "}";
    stream.close();
    // generate tree image
    // string cmd = "dot -Tpng graph.gv -o " + string(filename) + ".png";
    // system((const char *)cmd.c_str());
}

Graph *makeGraph()
{
    ifstream input;
    input.open("graph.txt");
    int u, v, wght;
    int vertices = 0, edges = 0;
    input >> vertices;
    input >> edges;
    Graph *graph = new Graph(vertices);
    while (input >> u)
    {
        input >> v;
        input >> wght;
        graph->add_edge(u, v, wght);
    }
    return graph;
}

int main()
{
    Graph *graph = makeGraph();
    // graph->print_graph();
    // graph->dfs();
    graph->dijkstra(0);
    return 0;
}
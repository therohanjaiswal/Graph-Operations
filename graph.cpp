#include <bits/stdc++.h>
#include "min_heap.h"
#include <stdlib.h>

using namespace std;

int timer = 1;
int component = 0;

class CompGraph
{
private:
    vector<pair<int, int>> *adj = nullptr;
    int V;

public:
    CompGraph(int vertices)
    {
        V = vertices;
        adj = new vector<pair<int, int>>[V];
    }
    void add_comp_edge(int, int);
    void print_comp_graph();
    bool topological_sort();
    void topological_sort_util(int, bool *, stack<int> &);
};

class Graph
{
private:
    vector<pair<int, int>> *adj = nullptr;
    int V;
    int e;

    int min_distance(int *, bool *);
    void traverse_dfs(bool *, int *, int *, string &, string &, int);
    void scc_utils(int, int *, int *, stack<int> *, bool *, int *);
    void print_scc(int *);

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
    int *scc_tarjan();
    bool is_semiconnected();
};

void CompGraph::print_comp_graph()
{

    ofstream stream;
    string str = "comp_graph.gv";
    stream.open(str);
    stream << "digraph g { \nranksep = \"equally\";\nrankdir = LR;\n";

    string node_string = "";  // to store the details of every node
    string edges_string = ""; // to store the pointers details
    ostringstream oss1;
    ostringstream oss2;

    for (int i = 0; i < V; i++)
    {
        oss1 << i << "[color = \"blue\"];\n";
        node_string.append(oss1.str());
        for (auto it = adj[i].begin(); it != adj[i].end(); it++)
        {
            oss2 << i << "->" << it->first << "[label=\"" << it->second << "\"];\n";
            edges_string.append(oss2.str());
            oss2.str("");
            oss2.clear();
        }
        oss1.str("");
        oss1.clear();
    }

    stream << node_string;
    stream << edges_string;
    stream << "}";
    stream.close();

    string cmd = "dot -Tpng comp_graph.gv -o comp_graph.png";
    system((const char *)cmd.c_str());
}

void CompGraph::add_comp_edge(int u, int v)
{
    adj[u].push_back(make_pair(v, 0));
}

// A recursive function used by topologicalSort
void CompGraph::topological_sort_util(int v, bool *visited, stack<int> &my_stack)
{
    // Mark the current node as visited.
    visited[v] = true;

    // Recur for all the vertices adjacent to this vertex
    for (auto it = adj[v].begin(); it != adj[v].end(); ++it)
        if (!*(visited + it->first))
            topological_sort_util(it->first, visited, my_stack);

    // Push current vertex to stack which stores result
    my_stack.push(v);
}

// The function to do Topological Sort. It uses recursive topologicalSortUtil()
bool CompGraph::topological_sort()
{
    stack<int> my_stack;
    int *temp = new int[component];
    // Mark all the vertices as not visited
    bool *visited = new bool[V];
    for (int i = 0; i < V; i++)
        visited[i] = false;

    // Call the recursive helper function to store Topological
    // Sort starting from all vertices one by one
    for (int i = 0; i < V; i++)
        if (visited[i] == false)
            topological_sort_util(i, visited, my_stack);

    // Print contents of stack
    int i = 0;
    while (my_stack.empty() == false)
    {
        *(temp + i++) = my_stack.top();
        my_stack.pop();
    }

    for (int i = 0; i < component - 1; ++i)
    {
        vector<pair<int, int>> u = *(adj + *(temp + i));
        bool has_edge = false;
        for (auto it = u.begin(); it != u.end(); ++it)
        {
            if (it->first == *(temp + i + 1))
            {
                has_edge = true;
                break;
            }
        }

        if (!has_edge)
            return false;
    }
    return true;
}

bool Graph::is_semiconnected()
{
    int *comp = scc_tarjan();
    CompGraph *comp_graph = new CompGraph(component);

    for (int i = 0; i < V; ++i)
        for (auto it = adj[i].begin(); it != adj[i].end(); ++it)
            if (*(comp + i) != *(comp + it->first))
                comp_graph->add_comp_edge(*(comp + i) - 1, *(comp + it->first) - 1);

    comp_graph->print_comp_graph();
    return comp_graph->topological_sort();
}

void Graph::scc_utils(int u, int *disc, int *low, stack<int> *st, bool *stackMember, int *comp_arr)
{
    // Initialize discovery time and low value
    *(disc + u) = *(low + u) = ++timer;
    st->push(u);
    *(stackMember + u) = true;

    // Go through all vertices adjacent to this
    for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
    {
        int v = it->first; // v is current adjacent of 'u'

        // If v is not visited yet, then recur for it
        if (*(disc + v) == -1)
        {
            scc_utils(v, disc, low, st, stackMember, comp_arr);

            // Check if the subtree rooted with 'v' has a
            // connection to one of the ancestors of 'u'
            // Case 1 (per above discussion on Disc and Low value)
            *(low + u) = min(*(low + u), *(low + v));
        }

        // Update low value of 'u' only if 'v' is still in stack (i.e. it's a back edge, not cross edge).
        else if (*(stackMember + v) == true)
            *(low + u) = min(*(low + u), *(disc + v));
    }

    // head node found, pop the stack and print an SCC
    int w = 0; // To store stack extracted vertices
    if (*(low + u) == *(disc + u))
    {
        ++component;
        while (st->top() != u)
        {
            w = (int)st->top();
            // cout << w << " ";
            *(comp_arr + w) = component;
            *(stackMember + w) = false;
            st->pop();
        }
        w = (int)st->top();
        // cout << w << "\n";
        *(comp_arr + w) = component;
        *(stackMember + w) = false;
        st->pop();
    }
}

int *Graph::scc_tarjan()
{
    int *disc = new int[V];
    int *low = new int[V];
    bool *stackMember = new bool[V];
    stack<int> *st = new stack<int>();
    timer = 0;
    component = 0;
    int *comp_arr = new int[V];
    // Initialize disc and low, and stackMember arrays
    for (int i = 0; i < V; i++)
    {
        *(disc + i) = -1;
        *(low + i) = -1;
        *(stackMember + i) = false;
    }

    // Call the recursive helper function to find strongly
    // connected components in DFS tree with vertex 'i'
    for (int i = 0; i < V; i++)
        if (*(disc + i) == -1)
            scc_utils(i, disc, low, st, stackMember, comp_arr);

    // print_scc(comp_arr);
    return comp_arr;
}

void Graph::print_scc(int *comp_arr)
{
    for (int i = 0; i < V; i++)
        *(comp_arr + i) %= 24;

    ofstream stream;
    string str = "scc.gv";
    stream.open(str);
    stream << "digraph g { \nranksep = \"equally\";\nrankdir = LR;\n";

    string node_string = "";  // to store the details of every node
    string edges_string = ""; // to store the pointers details
    ostringstream oss1;
    ostringstream oss2;

    // string color[] = {"red", "blue", "green", "yellow", "orange", "pink", "cyan", "magenta", "brown", "gray", "black", "violet", "navyblue", "maroon", "aqua", "khaki", "crimson", "gold"};
    string colors[] = {"red", "blue", "green", "yellow", "magenta", "aqua", "brown", "crimson", "coral", "darkgoldenrod", "darkgreen", "darkkhaki", "darkorange", "gold", "orange", "lightblue", "lightcoral", "salmon", "khaki", "maroon", "olive", "purple", "teal", "darkred"};
    for (int i = 0; i < V; i++)
    {
        oss1 << i << "[color = \"" << colors[*(comp_arr + i)] << "\"];\n";
        node_string.append(oss1.str());
        oss1.str("");
        oss1.clear();

        for (auto it = adj[i].begin(); it != adj[i].end(); it++)
        {
            if (*(comp_arr + i) == *(comp_arr + it->first))
                oss2 << i << "->" << it->first << "[label=\"" << it->second << "\"];\n";

            else
                oss2 << i << "->" << it->first << "[label=\"" << it->second << "\" color=\"gray\"];\n";
            edges_string.append(oss2.str());
            oss2.str("");
            oss2.clear();
        }
    }

    stream << node_string;
    stream << edges_string;
    stream << "}";
    stream.close();

    string cmd = "dot -Tpng scc.gv -o scc.png";
    system((const char *)cmd.c_str());
}

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

    string cmd = "dot -Tpng dfs.gv -o dfs.png";
    system((const char *)cmd.c_str());
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
            if (*(end + it->first) == 0) // back
                oss << i << "->" << it->first << "[label=\" " << it->second << "\" color=\"green\"];\n";
            else if (*(start + i) < *(start + it->first)) // forward
                oss << i << "->" << it->first << "[label=\" " << it->second << "\" color=\"red\"];\n";
            else // cross
                oss << i << "->" << it->first << "[label=\" " << it->second << "\" color=\"blue\"];\n";
            edge_str.append(oss.str());
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
                        { // back
                            oss << s << "->" << it->first << "[label=\"" << it->second << "\" color=\"green\"];\n";
                            edges_string.append(oss.str());
                        }
                        else if (*(exp_time + s) > *(exp_time + it->first))
                        { // forward
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
    string cmd = "dot -Tpng graph.gv -o graph.png";
    system((const char *)cmd.c_str());
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
    graph->dfs();
    // graph->dijkstra(0);
    // graph->scc_tarjan();
    cout << graph->is_semiconnected();
    return 0;
}
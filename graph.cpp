#include <bits/stdc++.h>
#include "min_heap.h"
#include <stdlib.h>

using namespace std;

int timer = 0;                    // timer
int component = 0;                // total number of strongly connected components
unordered_map<int, int> edge_map; // store unique edges of component DAG

/* CompGraph class reprsents Component DAG of the given graph */
class CompGraph
{
private:
    vector<int> *adj = nullptr; // vector array of int representing edges
    int V;                      // number of vertices ( equals to number of strongly connected components in the given graph)

public:
    // constructor
    CompGraph(int vertices)
    {
        V = vertices;
        adj = new vector<int>[V];
    }
    void add_comp_edge(int, int);
    void print_comp_graph();
    bool topological_sort();
    void topological_sort_util(int, bool *, stack<int> &);
    void map_edges();

    // destructor
    ~CompGraph()
    {
        delete adj;
    }
};

/* Graph class representation */
class Graph
{
private:
    // vector array of pairs representing edges and their weights
    vector<pair<int, int>> *adj = nullptr;
    int V; // number of vertices
    int e; // number of edges

    int min_distance(int *, bool *);
    void traverse_dfs(bool *, int *, int *, string &, string &, int);
    void scc_utils(int, int *, int *, stack<int> *, bool *, int *);
    void print_scc(int *, const char *filname);
    void print_dijkstra(int *, int *, const char *);

public:
    // constructor
    Graph(int vertices)
    {
        V = vertices;
        adj = new vector<pair<int, int>>[V];
    }

    void add_edge(int, int, int);
    void print_graph(const char *);
    void perform_dfs(const char *);
    void perform_dijkstra(int, const char *);
    int *scc_tarjan(const char *);
    bool is_semiconnected(const char *);
    Graph *compress_graph();

    // destructor
    ~Graph()
    {
        delete adj;
    }
};

/* This function generates iamge showing component DAG 
    of the given graph */
void CompGraph::print_comp_graph()
{
    // forming inital part of comp_dag.gv file
    ofstream stream;
    string str = "comp_dag.gv";
    stream.open(str);
    stream << "digraph g { \nranksep = \"equally\";\nrankdir = LR;\n";

    string node_string = "";  // to store the veretx details
    string edges_string = ""; // to store edge details
    ostringstream oss1, oss2;

    // iterate through all the vertices of component DAG
    for (int u = 0; u < V; u++)
    {
        oss1 << u << "[color = \"blue\"];\n";
        node_string.append(oss1.str()); // add vertex details

        // traverse adjacent verteices of vertex u
        for (auto it = adj[u].begin(); it != adj[u].end(); it++)
        {
            oss2 << u << "->" << *it << ";\n";
            edges_string.append(oss2.str()); // add edge details
            oss2.str("");
            oss2.clear();
        }
        oss1.str("");
        oss1.clear();
    }

    // forming comp_dag.gv file
    stream << node_string;
    stream << edges_string;
    stream << "}";
    stream.close();

    // generate component DAG image of the graph
    string cmd = "dot -Tpng comp_dag.gv -o comp_dag.png";
    system((const char *)cmd.c_str());
}

/* This function adds an edge to component graph */
void CompGraph::add_comp_edge(int u, int v)
{
    adj[u].push_back(v);
}

/* Utility function for topological sort */
void CompGraph::topological_sort_util(int v, bool *visited, stack<int> &stck)
{
    visited[v] = true; // Mark vertex v as visited.

    // Recur for all the vertices adjacent to vertex v
    for (auto it = adj[v].begin(); it != adj[v].end(); ++it)
        if (!*(visited + *it))
            topological_sort_util(*it, visited, stck);

    // Push vertex v to stack which stores result
    stck.push(v);
}

/* This function performs topological sort on component DAG using 
    a utility function topological_sort_util */
bool CompGraph::topological_sort()
{
    stack<int> stck;                // stack to store the result of topological sort
    int *temp = new int[component]; // arrray to store topological sequence of DAG
    bool *visited = new bool[V];    // array to check whther a vertex is visited or not

    // All vertices are unvisited initially
    for (int i = 0; i < V; i++)
        visited[i] = false;

    // Recursively call utility function to perform topological
    //  sort from all vertices if the vertex is not visited yet
    for (int i = 0; i < V; i++)
        if (visited[i] == false)
            topological_sort_util(i, visited, stck);

    // store content of stack in temp array and pop from stack
    int i = 0;
    while (stck.empty() == false)
    {
        *(temp + i++) = stck.top();
        stck.pop();
    }

    // iterate through the component DAG and check whether
    // there is an edge or not between adjacent vertices of topological
    // sort sequence of component DAG
    for (int i = 0; i < component - 1; ++i)
    {
        vector<int> u = *(adj + *(temp + i));
        bool has_edge = false;
        for (auto it = u.begin(); it != u.end(); ++it)
        {
            // if there is an between adjacent vertices of of topological
            // sort sequence of component DAG
            if (*it == *(temp + i + 1))
            {
                has_edge = true;
                break;
            }
        }

        // if there is no edge
        if (!has_edge)
            return false;
    }
    return true;
}

/* This function stores unique edges between the components */
void CompGraph::map_edges()
{
    edge_map.clear(); // clear map

    // Iterate through all the vertices of component DAG
    for (int u = 0; u < V; ++u)
    {
        // Traverse all the adjacent vertices of vertex u and creates a unique key
        // for the adjacent edge and store it into unordered map
        for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
        {
            int key = (1051 * u) + (1151 * *it);
            if (edge_map.find(key) == edge_map.end())
                edge_map.insert({key, 1});
        }
    }
}

/* This function creates a new graph G`(V, E`) with same strongly connected
components, same component and edges E` as less as possible. */
Graph *Graph ::compress_graph()
{
    Graph *graph = new Graph(V);   // create new compressed graph object
    int *comp = scc_tarjan("scc"); // get the component dag

    CompGraph *comp_graph = new CompGraph(component);

    // add all edges within scc
    // iterate through all the vertices
    for (int u = 0; u < V; ++u)
    {
        // traverse all the adjacent vertices of vertex u
        for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
        {
            // if vertex u and vertex v belongs to same component
            // then add an edge in compressed graph
            if (*(comp + u) == *(comp + it->first))
                graph->add_edge(u, it->first, it->second);
            else // otherwise add them to component graph
                comp_graph->add_comp_edge(*(comp + u) - 1, *(comp + it->first) - 1);
        }
    }

    // add compressed edges inter scc
    // hash the edges
    comp_graph->map_edges();

    // iterate through all the vertices of given graph
    for (int u = 0; u < V; ++u)
    {
        for (auto it = adj[u].begin(); it != adj[u].end(); ++it) //for each adjacent node of the vertex
        {
            int v = it->first;              // adjacent vertex of u
            int w = it->second;             // weight of each edge
            if (*(comp + u) != *(comp + v)) //if source and destination don't belong to the same component
            {
                int key = (1051 * (*(comp + u) - 1)) + (1151 * (*(comp + v) - 1)); //get the key at dag edge map

                //if the key is present, means edge is present
                if (edge_map.find(key) != edge_map.end())
                {
                    graph->add_edge(u, v, edge_map.at(key)); //add the edge to minimal graph only if it is present in the dag edge map
                    edge_map.erase(key);                     //remove the edge from dag edge map
                }
            }
        }
    }
    return graph;
}

/* This function finds out whether given graph is semi-connected or not */
bool Graph::is_semiconnected(const char *filename)
{
    // scc_tarjan returns component array which represents
    // which vertex belongs to which component number
    int *comp_arr = scc_tarjan(filename);

    // create component DAG
    CompGraph *comp_graph = new CompGraph(component);

    // iterate through all the vertices
    for (int u = 0; u < V; ++u)
        // traverse all the adjacent vertices of vertex u
        for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
            // if vertex u and vertex v belongs to different component
            // then add an edge in component DAG
            if (*(comp_arr + u) != *(comp_arr + it->first))
                comp_graph->add_comp_edge(*(comp_arr + u) - 1, *(comp_arr + it->first) - 1);

    comp_graph->print_comp_graph();        // print component DAG
    return comp_graph->topological_sort(); // perform topological sort on DAG and return
}

/* Utility function to find out strongly connected components */
void Graph::scc_utils(int u, int *disc, int *low, stack<int> *stck, bool *in_stack, int *comp_arr)
{
    *(disc + u) = *(low + u) = ++timer; // Initialization of discovery time and low value with current time
    stck->push(u);                      // push that into stack
    *(in_stack + u) = true;             // set in_stack to true as u is now present in stack

    // Traverse all adjacency vertices of u
    for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
    {
        int v = it->first; // v is current adjacent of vertex u

        // If v is not visited yet, then recur for it
        if (*(disc + v) == -1)
        {
            scc_utils(v, disc, low, stck, in_stack, comp_arr);

            // check if v has connection with one of the ancestor of u
            *(low + u) = min(*(low + u), *(low + v));
        }

        // if v is still in stack then there is a back edge
        else if (*(in_stack + v) == true)
            *(low + u) = min(*(low + u), *(disc + v));
    }

    // head node of one of the strongly connected component is met
    if (*(low + u) == *(disc + u))
    {
        int w = 0;
        ++component; // increment total number of scc

        // pop stack upto vertex u
        while (stck->top() != u)
        {
            w = (int)stck->top();
            *(comp_arr + w) = component; // set component number
            *(in_stack + w) = false;     // set in_stack to false as w is poped now
            stck->pop();
        }

        // perform same thing last vertex present in stack
        w = (int)stck->top();
        *(comp_arr + w) = component;
        *(in_stack + w) = false;
        stck->pop();
    }
}

/* This function performs tarjan algorithm to fins strongly connected 
    components of the given graph */
int *Graph::scc_tarjan(const char *filename)
{
    int *disc = new int[V]; // array to store dicovery time of each vertex
    int *low = new int[V];
    bool *in_stack = new bool[V];        // array to check whether a vertex is in stack or not
    stack<int> *stck = new stack<int>(); // stack to push vertex

    timer = 0;                  // initialize timer to 0
    component = 0;              // initialize total number of strongly connected components to 0
    int *comp_arr = new int[V]; // array to store component number of each scc

    // Initialization
    for (int i = 0; i < V; i++)
    {
        *(disc + i) = -1;
        *(low + i) = -1;
        *(in_stack + i) = false;
    }

    // Recursive call to find strongly connected component
    // using DFS tree
    for (int i = 0; i < V; i++)
        if (*(disc + i) == -1)
            scc_utils(i, disc, low, stck, in_stack, comp_arr);

    print_scc(comp_arr, filename); // printing strongly connected components
    return comp_arr;               // return component array
}

/* This function generates image showing strongly connected components 
    of the given graph */
void Graph::print_scc(int *comp_arr, const char *filename)
{
    // taking mod 24 of component number
    for (int i = 0; i < V; i++)
        *(comp_arr + i) %= 24;

    // forming initial part of "scc.gv file
    ofstream stream;
    string str = "scc.gv";
    stream.open(str);
    stream << "digraph g { \nranksep = \"equally\";\nrankdir = LR;\n";

    string node_string = "";  // to store the details of every vertex
    string edges_string = ""; // to store the edge details
    ostringstream oss1, oss2;

    // colors array
    string colors[] = {"red", "blue", "green", "yellow", "magenta", "aqua", "brown", "crimson", "coral", "darkgoldenrod", "darkgreen", "darkkhaki", "darkorange", "gold", "orange", "lightblue", "lightcoral", "salmon", "khaki", "maroon", "olive", "purple", "teal", "darkred"};

    // iterate through all vertices
    for (int u = 0; u < V; u++)
    {
        // adding vertex details
        oss1 << u << "[color = \"" << colors[*(comp_arr + u)] << "\"];\n";
        node_string.append(oss1.str());
        oss1.str("");
        oss1.clear();

        // Traverse all adjacent vetices of vertex u
        for (auto it = adj[u].begin(); it != adj[u].end(); it++)
        {
            // if the component number of vertex u and it's adjacent vertex v are different
            // then both belongs to different components
            if (*(comp_arr + u) == *(comp_arr + it->first))
                oss2 << u << "->" << it->first << "[label=\"" << it->second << "\"];\n";

            // if the component number of vertex u and it's adjacent vertex v are different
            // then both belongs to same components
            else
                oss2 << u << "->" << it->first << "[label=\"" << it->second << "\" color=\"gray\"];\n";
            edges_string.append(oss2.str()); // adding edge details
            oss2.str("");
            oss2.clear();
        }
    }

    // forming scc.gv file
    stream << node_string;
    stream << edges_string;
    stream << "}";
    stream.close();

    // generate graph image with filename
    string cmd = "dot -Tpng scc.gv -o " + string(filename) + ".png ";
    system((const char *)cmd.c_str());
}

/* This function generates image showing the shortest path
 to every node using dijkstra algorithm */
void Graph::print_dijkstra(int *par, int *dist, const char *filename)
{

    // forming initial part of "dijkstra.gv file
    ofstream stream;
    string str = "dijkstra.gv";
    stream.open(str);
    stream << "digraph g { \nranksep = \"equally\";\nrankdir = LR;\n";

    string node_string = "";  // store node details
    string edges_string = ""; // store pointers details
    ostringstream oss1, oss2;

    // iterate for every vertex
    for (int u = 0; u < V; u++)
    {
        oss1 << u << "[label=\"" << u << "(" << *(dist + u) << ")\" color=\"blue\"];\n";
        node_string.append(oss1.str()); // add vertex details
        oss1.str("");
        oss1.clear();

        // traverse adjacent vertices of u
        for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
        {
            if (*(par + it->first) == u)
                oss2 << u << "->" << it->first << "[label=\"" << it->second << "\" color=\"blue\"];\n";
            else
                oss2 << u << "->" << it->first << "[label=\"" << it->second << "\" color=\"gray\"];\n";
            edges_string.append(oss2.str()); // add edge details
            oss2.str("");
            oss2.clear();
        }
    }

    // forming dijkstra.gv file
    stream << node_string;
    stream << edges_string;
    stream << "}";
    stream.close();

    // generate graph image with filename
    string cmd = "dot -Tpng dijkstra.gv -o " + string(filename) + ".png ";
    system((const char *)cmd.c_str());
}

/* This function takes the source vertex argument and perform dijkstra algorithm
to find out shortest path fom source to every vertex */
void Graph::perform_dijkstra(int src, const char *filename)
{
    int *dist = new int[V]; // array to store distance of each vertex from source vertex
    int *par = new int[V];  // array to store parent of each vertex

    // create min heap with V vertices acting as priority queue
    MinHeap *minHeap = new MinHeap(V);

    // Initialization of  min heap with all vertices. dist value of all vertices
    for (int v = 0; v < V; ++v)
    {
        *(dist + v) = INT_MAX;
        *(minHeap->array + v) = minHeap->create_minheap_node(v, *(dist + v));
        *(minHeap->pos + v) = v;
        *(par + v) = -1;
    }

    // set distance value of src vertex as 0 so that it is extracted first
    *(minHeap->array + src) = minHeap->create_minheap_node(src, *(dist + src));
    *(minHeap->pos + src) = src;
    *(dist + src) = 0;

    // reorganize minheap
    minHeap->decrease_key(src, *(dist + src));

    // Set size of minheap to V
    minHeap->size = V;

    // Iterate until the min heap is not empty i.e., until all edges are relaxed
    while (minHeap->size != 0)
    {
        // Extract the vertex with minimum distance value
        HeapNode *min_heap_node = minHeap->extract_min();

        // Store the extracted vertex number
        int u = min_heap_node->v;

        // Travrese through all adjacent vertices of u and update their distance values
        for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
        {
            int v = it->first; // adjacent vertex of u

            // relax edge u -> v and update distance of v from source
            if (minHeap->is_in_min_heap(v) && dist[u] != INT_MAX && it->second + dist[u] < dist[v])
            {
                dist[v] = dist[u] + it->second;
                minHeap->decrease_key(v, dist[v]); // reorganize min heap
                *(par + v) = u;                    // set u as parent of v
            }
        }
    }

    // print the calculated shortest distances
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < V; ++i)
        printf("%d \t\t %d\n", i, dist[i]);

    printf("Vertex   Parent\n");
    for (int i = 0; i < V; ++i)
        printf("%d \t %d\n", i, *(par + i));

    print_dijkstra(par, dist, filename);
}

/* This function performs dfs traversal of the graph and generates the 
image representing the dfs traversal of the graph with the given filename*/
void Graph::perform_dfs(const char *filename)
{
    // forming initial part of "dfs.gv file"
    ofstream stream;
    string str = "dfs.gv";
    stream.open(str);
    stream << "digraph g { \nranksep = \"equally\";\nrankdir = LR;\n";

    string node_string = ""; // store vertices details
    string edge_string = ""; // store edges details
    ostringstream oss;
    timer = 0; // initialize timer

    bool *visited = new bool[V];  // array to check whether a vertex is visited or not
    int *start_time = new int[V]; // array to store start time of exploration
    int *end_time = new int[V];   // array to store end time of exploration

    // initialization
    for (int i = 0; i < V; i++)
    {
        visited[i] = false;
        start_time[i] = 0;
        end_time[i] = 0;
    }

    // iterate through every vertex
    for (int i = 0; i < V; i++)
        if (*(visited + i) == false)
            traverse_dfs(visited, start_time, end_time, edge_string, node_string, i);

    stream << node_string;
    stream << edge_string;
    stream << "}";
    stream.close();

    // generate dfs traversal image of the graph
    string cmd = "dot -Tpng dfs.gv -o " + string(filename) + ".png";
    system((const char *)cmd.c_str());
}

/* Utility function to perform dfs traversal */
void Graph::traverse_dfs(bool *visited, int *start, int *end, string &edge_str, string &node_str, int i)
{
    ostringstream oss, oss1;

    *(visited + i) = true;  // set ith vertex as visited
    *(start + i) = timer++; // set start exploration time of ith vertex

    oss1 << i << "[label = \"" << i << "(" << *(start + i) << ",";

    // iterate through adjacent vertices of ith vertex
    for (auto it = adj[i].begin(); it != adj[i].end(); ++it)
    {
        // if adjacent vertex of ith vertex is not visited, recursively call for that vertex
        if (*(visited + it->first) == false)
        {
            oss << i << "->" << it->first << "[label=\" " << it->second << " \"];\n";
            edge_str.append(oss.str()); // append edge detail
            oss.str("");
            oss.clear();
            traverse_dfs(visited, start, end, edge_str, node_str, it->first);
        }
        else // if adjacent vertex of ith vertex is already visited
        {
            // Back Edge
            if (*(end + it->first) == 0)
                oss << i << "->" << it->first << "[label=\" " << it->second << "\" color=\"green\"];\n";

            // Forward Edge
            else if (*(start + i) < *(start + it->first))
                oss << i << "->" << it->first << "[label=\" " << it->second << "\" color=\"red\"];\n";

            // Cross Edge
            else
                oss << i << "->" << it->first << "[label=\" " << it->second << "\" color=\"blue\"];\n";

            edge_str.append(oss.str()); // append edge detail
            oss.str("");
            oss.clear();
        }
    }

    *(end + i) = timer++; // set end exploration time of ith vertex
    oss1 << *(end + i) << ")\"];\n";
    node_str.append(oss1.str()); // append node details
    oss1.str("");
    oss1.clear();
}

/* This function adds an edge with source u, destination v and 
 weight wt to the graph*/
void Graph::add_edge(int u, int v, int wt)
{
    adj[u].push_back(make_pair(v, wt)); // add edge to the graph
}

/* This function creates a png file representing the graph with the given filename */
void Graph::print_graph(const char *filename)
{
    // forming initial part of "graph.gv file"
    ofstream stream;
    string str = "graph.gv";
    stream.open(str);
    stream << "digraph g { \nranksep = \"equally\";\nrankdir = LR;\n";

    string node_string = "";  // store vertex details
    string edges_string = ""; // store edge details
    ostringstream oss1, oss2;

    // Iterate through all the vertices
    for (int u = 0; u < V; u++)
    {
        // adding vertex details
        oss1 << u << "[color = \"black\"];\n";
        node_string.append(oss1.str());
        oss1.str("");
        oss1.clear();
        // iterate through the adjacent vertex of sth vertex
        for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
        {
            oss2 << u << "->" << it->first << "[label=\"" << it->second << "\" color=\"black\"];\n";
            edges_string.append(oss2.str());
            oss2.str("");
            oss2.clear();
        }
    }

    stream << node_string;
    stream << edges_string;
    stream << "}";
    stream.close();

    // generate graph image with filename
    string cmd = "dot -Tpng graph.gv -o " + string(filename) + ".png ";
    system((const char *)cmd.c_str());
}

/* This function reads data from graph.txt and create graph*/
Graph *make_graph()
{
    ifstream input;
    input.open("graph.txt"); // open graph.txt to read data

    // u = source vertex of an edge, v = destination vertex of an edge, wght = weight of an edge
    int u, v, wght;
    int vertices = 0, edges = 0; // vertices = total number of vertices, edges = total number of edges
    input >> vertices;           // read total number of vertices
    input >> edges;              // eread total number of edges

    Graph *graph = new Graph(vertices); // create graph object

    // read graph.txt and add edges in the graph
    while (input >> u)
    {
        input >> v;
        input >> wght;
        graph->add_edge(u, v, wght);
    }
    return graph; // return graph object
}

int main()
{
    Graph *graph = make_graph();
    string filename;
    bool iteration = true;
    while (iteration)
    {
        int choice = 0, source;
        Graph *compressed_graph;
        cout << "\n=======================================";
        cout << "\n\tGraph Implementation";
        cout << "\n=======================================";
        cout << "\n1. Perform DFS traversal and classify edges.";
        cout << "\n2. Find out all strongly connected components.";
        cout << "\n3. Find out a subgaraph G`(V, E`) of G(V, E) with same strongly connected components, same component and E` as less as possible. ";
        cout << "\n4. Find out whether G is semi-connected or not.";
        cout << "\n5. Perform Dijkstra algorithm to find out shortest path from source to all vertices.";
        cout << "\n6. Print Graph";
        cout << "\n7. Quit.";
        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "\nEnter a filename: ";
            cin >> filename;
            graph->perform_dfs(filename.c_str());
            cout << filename << ".png has been generated!\n";
            break;

        case 2:
            cout << "\nEnter a filename: ";
            cin >> filename;
            graph->scc_tarjan(filename.c_str());
            cout << filename << ".png has been generated!\n";
            break;

        case 3:
            cout << "\nEnter a filename: ";
            cin >> filename;
            compressed_graph = graph->compress_graph();
            compressed_graph->print_graph(filename.c_str());
            cout << filename << ".png and scc.png has been generated!\n";
            break;

        case 4:
            cout << "\nEnter a filename: ";
            cin >> filename;
            graph->is_semiconnected(filename.c_str());
            cout << filename << ".png and comp_dag.png has been generated!\n";
            break;

        case 5:
            cout << "\nEnter a filename: ";
            cin >> filename;
            cout << "\nEnter a source vertex: ";
            cin >> source;
            graph->perform_dijkstra(source, filename.c_str());
            cout << filename << ".png has been generated!\n";
            break;

        case 6:
            cout << "\nEnter a filename: ";
            cin >> filename;
            graph->print_graph(filename.c_str());
            cout << filename << ".png has been generated!\n";
            break;

        case 7:
            iteration = false;
            break;

        default:
            cout << "\nInvalid input";
            break;
        }
    }
    return 0;
}
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <fstream>
#include <cmath>
#include <time.h>

using namespace std;

// TreapNode class represents Treap node structure
class TreapNode
{
private:
    int key;               // store element
    int priority;          // stores priority of the node
    TreapNode *leftChild;  // points to left child
    TreapNode *rightChild; // points to right child

public:
    TreapNode(int value, int pr)
    {
        key = value;
        priority = pr;
        leftChild = nullptr;
        rightChild = nullptr;
    }
    friend class Treap;
};

/* Treap class.
This tree consist of a dummy node and the actual 
Treap exists as the right child of the dummy node*/
class Treap
{
private:
    TreapNode *head;         // head points to dummy node
    int totalKeyComparisons; // store total number of key comparisons
    int totalRotations;      // stores total number of rotations
    void deleteTreapLeafNode(TreapNode *, TreapNode *);
    void rotateLeft(TreapNode *, TreapNode *);
    void rotateRight(TreapNode *, TreapNode *);
    int getHeightUtils(TreapNode *);
    void getAvgTreeHeightUtils(TreapNode *, float &, int &);

public:
    /* constructor initializes the dummy node with INT_MAX, 
    totalKeyComparisons and totalRotations to zero*/
    Treap()
    {
        head = new TreapNode(INT_MAX, INT_MAX);
        totalKeyComparisons = 0;
        totalRotations = 0;
    }

    void insertTreapNode(int, int);
    void deleteTreapNode(int);
    bool treapSearch(int);
    void printTree(const char *);
    int getTreeHeight();
    float getAvgTreeHeight();
    int getTotalKeyComparisons();
    int getTotalRotations();

    // destructor
    ~Treap()
    {
        delete head;
    }
};

/* This function performs insertion in the Treap */
void Treap::insertTreapNode(int key, int priority)
{

    TreapNode *newNode = new TreapNode(key, priority); // creating new Treap node
    TreapNode *root = head->rightChild;                // root points to the right child of dummy node

    // first insertion
    if (root == nullptr)
    {
        head->rightChild = newNode; // make new node as the right child of dummy node
        return;
    }

    // stack to store all the elements from root to the parent of new node including dummy node
    stack<TreapNode *> path;
    path.push(head);        // push head
    TreapNode *prev = head; // points to parent node of curr
    TreapNode *curr = root; // curr is used for traversal
    while (curr != nullptr)
    {
        prev = curr;
        path.push(prev); // push prev into stack
        ++totalKeyComparisons;
        try
        {
            if (key > curr->key)         // if key is greater than curr->key
                curr = curr->rightChild; // move to right subtree of curr
            else if (key < curr->key)    // if key is smaller than curr->key
                curr = curr->leftChild;  // move to left subtree of curr
            else                         // if key already present in the tree
                throw key;               // throw exception
        }
        catch (int x)
        {
            cout << key << " is already present.\n";
            return;
        }
    }

    ++totalKeyComparisons;
    // attaching the newly inserted node
    if (key > prev->key)
        prev->rightChild = newNode;
    else
        prev->leftChild = newNode;

    // performing required rotations
    TreapNode *parent = path.top(); // parent points to parent of new node
    TreapNode *child = newNode;     // child points to new node
    TreapNode *gparent = nullptr;   // gparent points to grand parent of new node

    // iterate until stack is empty
    while (path.top() != head)
    {
        path.pop();
        // if priority of parent node is greater than child node
        if (parent->priority > child->priority)
        {
            // if child is left child of parent
            if (child == parent->leftChild)
                rotateRight(parent, child); // perform right rotation
            // if child is right child of parent
            else if (child == parent->rightChild)
                rotateLeft(parent, child); // perform right rotation

            gparent = path.top();
            // adjust child pointers of gparent and child
            if (gparent->leftChild == parent)
            {
                gparent->leftChild = child;
                child = gparent->leftChild;
            }
            else if (gparent->rightChild == parent)
            {
                gparent->rightChild = child;
                child = gparent->rightChild;
            }
            parent = gparent;
        }
    }
}

/* This function performs insertion in the Treap */
void Treap::deleteTreapNode(int k)
{
    TreapNode *curr = head->rightChild; // curr points to the right child of dummy node initially

    // is tree is already empty
    if (curr == nullptr)
    {
        cout << "\nTree is empty\n";
        return;
    }

    // if root is deleted
    if (curr->key == k)
        ++totalKeyComparisons;

    // finding the node with value k by traversing using curr
    TreapNode *prev = head;
    while (curr != nullptr && curr->key != k)
    {
        prev = curr;
        ++totalKeyComparisons;
        if (curr->key < k)
            curr = curr->rightChild;
        else
            curr = curr->leftChild;
    }

    // if k is absent
    try
    {
        if (curr == nullptr)
            throw k;
    }
    catch (int x)
    {
        cout << x << " is absent in the tree.\n";
        return;
    }

    // if the node with value k is already a leaf node
    if (curr->leftChild == nullptr && curr->rightChild == nullptr)
    {
        deleteTreapLeafNode(prev, curr); // delete that node
        return;
    }

    // if curr is not a leaf initially, then make it a leaf node by performing rotations
    while (curr->leftChild != nullptr || curr->rightChild != nullptr)
    {
        TreapNode *currChild; // currChild points to child of curr
        // if curr has single child
        if (curr->leftChild == nullptr || curr->rightChild == nullptr)
            currChild = curr->leftChild != nullptr ? curr->leftChild : curr->rightChild;
        else // curr has double child
            currChild = curr->leftChild->priority < curr->rightChild->priority ? curr->leftChild : curr->rightChild;

        // if currChild is left child of curr
        if (currChild == curr->leftChild)
            rotateRight(curr, currChild);       // perform right rotation
        else if (currChild == curr->rightChild) // if currChild is right child of curr
            rotateLeft(curr, currChild);        // perform right rotation

        // adjust prev pointer
        if (prev->leftChild == curr)
            prev->leftChild = currChild;
        else if (prev->rightChild == curr)
            prev->rightChild = currChild;
        prev = currChild;
    }

    // curr is leaf now, so delete it
    deleteTreapLeafNode(prev, curr);
}

/*Utility function to perform deletion of leaf node which is pointed by curr*/
void Treap::deleteTreapLeafNode(TreapNode *prev, TreapNode *curr)
{
    // prev points to parent of curr
    // if curr is left child of prev
    if (prev->leftChild == curr)
        prev->leftChild = nullptr;
    else // otherwise
        prev->rightChild = nullptr;
}

/*Utility function to perform left rotation*/
void Treap::rotateLeft(TreapNode *parent, TreapNode *child)
{
    // single rotation
    TreapNode *temp = child->leftChild;
    child->leftChild = parent;
    parent->rightChild = temp;
    ++totalRotations;
}

/*Utility function to perform right rotation*/
void Treap::rotateRight(TreapNode *parent, TreapNode *child)
{
    // single rotation
    TreapNode *temp = child->rightChild;
    child->rightChild = parent;
    parent->leftChild = temp;
    ++totalRotations;
}

/*This function searches for an element with value k in the Treap*/
bool Treap::treapSearch(int k)
{
    TreapNode *temp = head->rightChild; // temp points to root of the tree which is the right child of head node
    if (temp == nullptr)                // if tree is empty
        return false;

    // iterate until temp becomes null i.e., temp reaches leaf node
    while (temp != nullptr)
    {
        if (temp->key == k) // if k is found
            return true;
        else if (temp->key < k) // if k is greater than key of root
            temp = temp->rightChild;
        else // if k is smaller than key of root
            temp = temp->leftChild;
    }
    return false; // if k is not present in the tree
}

/*This function forms the "graph.gv" file which helps to generate the
    image of the tree*/
void Treap::printTree(const char *filename)
{
    // forming initial part of "graph.gv file"
    ofstream stream;
    string str = "graph.gv";
    stream.open(str);
    stream << "digraph g { \n\tnode[shape = record, height = .1];\n";
    stream << "\tnode" << INT_MAX << "[label = \"<l> | <d> Dummy Node | <r> \"];\n";

    TreapNode *root = head->rightChild; // root points to right child of dummy node i.e., root of actual TBST
    // when tree is empty
    if (root == nullptr)
    {
        cout << "Tree is empty" << endl;
    }
    else
    {
        string nodeString = "";    // to store the details of every node
        string pointerString = ""; // to store the pointers details
        ostringstream oss;
        queue<TreapNode *> q; // queue to perform level order traversal
        q.push(root);

        while (!q.empty())
        {
            TreapNode *node = q.front();
            // storing node details
            oss << "\tnode" << node->key << "[label = \"<l> | <d> " << node->key << "," << node->priority << " | <r>\"];\n";
            nodeString.append(oss.str());
            oss.str("");
            oss.clear();

            // Enqueue left child
            if (node->leftChild != nullptr)
            {
                q.push(node->leftChild);
                // storing node details
                oss << "\t\"node" << node->key << "\":l -> \"node" << node->leftChild->key << "\":d;\n";
                pointerString.append(oss.str());
                oss.str("");
                oss.clear();
            }

            // Enqueue right child
            if (node->rightChild != nullptr)
            {
                q.push(node->rightChild);
                // storing node details
                oss << "\t\"node" << node->key << "\":r -> \"node" << node->rightChild->key << "\":d;\n";
                pointerString.append(oss.str());
                oss.str("");
                oss.clear();
            }
            q.pop();
        }
        stream << nodeString;
        stream << "\t\"node" << head->key << "\":r -> \"node" << head->rightChild->key << "\":d;\n";
        stream << pointerString;
    }
    stream << "}";
    stream.close();

    // generate tree image
    string cmd = "dot -Tpng graph.gv -o " + string(filename) + ".png";
    system((const char *)cmd.c_str());
}

/*Utility function to compute the height of a node*/
int Treap::getHeightUtils(TreapNode *root)
{
    // base case
    if (root == nullptr || (root->leftChild == nullptr && root->rightChild == nullptr))
        return 0;
    return 1 + max(getHeightUtils(root->leftChild), getHeightUtils(root->rightChild)); // return height of tree
}

/*This function computes height of the resultant tree*/
int Treap::getTreeHeight()
{
    return getHeightUtils(head->rightChild);
}

/*Utility function to compute the average height of the tree using inorder traversal*/
void Treap::getAvgTreeHeightUtils(TreapNode *root, float &sum, int &totalNodes)
{
    // base case
    if (root == nullptr)
        return;

    getAvgTreeHeightUtils(root->leftChild, sum, totalNodes);  // compute the average height of the left subtree
    sum += getHeightUtils(root);                              // add height of each node
    ++totalNodes;                                             //increment total number of nodes
    getAvgTreeHeightUtils(root->rightChild, sum, totalNodes); // compute the average height of the right subtree
}

/*This function computes the average height of the tree*/
float Treap::getAvgTreeHeight()
{
    float sum = 0;      //store the sum heights of all nodes
    int totalNodes = 0; // store total number of nodes present in the resultant tree
    getAvgTreeHeightUtils(head->rightChild, sum, totalNodes);
    return sum / totalNodes; // returns average height of the resultant tree
}

/*This function returns the total number of key comparisons performed during insert and delete operation*/
int Treap::getTotalKeyComparisons()
{
    return totalKeyComparisons;
}

/*This function returns the total number of rotations performed during insert and delete operation*/
int Treap::getTotalRotations()
{
    return totalRotations;
}

/*
 * Sipos David Oliver 30423
 *
 * I implemented order statistics trees: a balanced binary search tree and a red-black tree.
 * These are augmented structures, each node contains an additional field which stores the size
 * of a subtree. The select operation returns the i-th smallest element in the tree.
 *
 * To build the BST I used a divide and conquer approach yielding O(n) complexity. The delete and
 * select operations on the BST have complexity O(logn).
 *
 * The red-black tree is built using the insert operation, which has complexity O(logn), resulting
 * in O(nlogn) total complexity. One could build the red black tree using the same approach as in the case
 * of the BST and then color the nodes. This approach would eliminate the challenge of the
 * assignment, so I chose the slower, but more complicated, insert operation. Delete and select
 * have complexity O(logn).
 *
 * For this assignment we don't have to balance the trees because we are not interested in
 * inserting elements after deletions thus the height of the tree doesn't increase. In order to perform
 * a correct deletion from the red black tree I implemented balancing to restore the tree's properties.
 *
 * Brief explanation of red black operations:
 *
 * A red black tree is a BST which has the following properties:
 * 1. Every node is either red or black.
 * 2. The root is black.
 * 3. Every leaf (NIL) is black.
 * 4. If a node is red, then both its children are black.
 * 5. For each node, all simple paths from the node to descendant leaves contain the same number of black nodes.
 *
 * Each operation is performed like regular BST operations followed by some fix-up procedures.
 *
 * Insert:
 * If we inserted a black node then the black height of the ancestors would be altered. A better choice
 * is to insert a red node. Properties 2 and 4 could get violated. If the tree is empty then inserting
 * a red node would cause the root to be red thus violating property 2. If the parent of the newly
 * inserted node is red then this node would violate property 4. To fix these problems we use an auxiliary
 * procedure, which goes up in the tree. The situations which occur are grouped in cases. Some
 * cases get transformed in other cases, some of them move the problem up in the tree,
 * and some of them solve the problem directly.
 *
 * Delete:
 * During deletion there are nodes which change their position in the tree. A change in the position of
 * a red node doesn't introduce violations. If a black node changes its position more problems arise.
 * Let z be the node we wish to delete, y be the node that will be deleted and moved to z's original
 * position taking z's color, and x the node which will move to y's original position.
 * The delete procedure keeps track of these nodes. Assume now that y is black. If y is the root and a
 * red child of y becomes the root property 2 is violated. If x and the new parent of x are red then property 4 is violated.
 * Moving the node y could alter the black heights of the ancestors violating property 5.
 * This violation is hard to solve so we transform it into another problem.
 * x will move into y's original position so adding a black count to x fixes property 5
 * but violates property 1. x will be a doubly-black or a red-black node, the additional black is represented
 * by the fact that x points to the node. Looking at the while loop in the fix-up procedure we can observe
 * that the loop stops when x is the root or x is a red-black node. At the end of the procedure x will
 * be colored black fixing property 2 and 4. To fix property 1 the idea is to move the extra black up
 * in the tree until one of the following occurs:
 * 1. x is red-black => coloring x black fixes the problem
 * 2. x is the root => we remove the extra black from the tree.
 * 3. some recolorings and rotations solve the problem.
 * Just like when inserting we have cases behaving in a similar manner.
 *
 * The cases defined for each operation exhaust all possible situations.
 *
 * Looking at the charts we can see that it is more expensive to maintain the red-black tree, but we can't
 * ignore the fact that the analyzed implementation of the BST is specifically tailored for this assignment,
 * and the implementation of the red black tree is a general one. Selection has the same performance for both
 * types of trees, differences occur due to the fact that the red-black tree changes its shape.
 */
#include <iostream>
#include <random>
#include <cstring>
#include "Profiler.h"

#define MIN_N 100
#define MAX_N 10000
#define STEP 100
#define TEST 5

using namespace std;

struct RBNode
{
    int key, size;
    RBNode *parent, *left, *right;
    bool color;
};

struct BTNode
{
    int key, size;
    BTNode *left, *right;
};

Profiler p("DynamicOrderStatistics");
RBNode *NIL;
int problem_size;
char comp[30], assign[30];
default_random_engine engine(time(0));

void leftRotateRB(RBNode **root, RBNode *x);
void rightRotateRB(RBNode **root, RBNode *x);
void insertRBFix(RBNode **root, RBNode *z);
void insertRB(RBNode **root, int key);
void transplantRB(RBNode **root, RBNode *u, RBNode *v);
RBNode* treeMinRB(RBNode *root);
void deleteRBFix(RBNode **root, RBNode *x);
void deleteRB(RBNode **root, int key);
RBNode* buildTreeRB(int *v, int n);
int selectRB(RBNode *root, int i);
void prettyPrintRB(RBNode *root, int level);
void destroyRB(RBNode *root);
BTNode* buildTreeBT(int *v, int i, int j);
BTNode* treeMinBT(BTNode *root);
BTNode* deleteBT(BTNode *root, int key);
int selectBT(BTNode *root, int i);
void prettyPrintBT(BTNode *root, int level);
void destroyBT(BTNode *root);
void printArray(int *v, int n);
int getRandomNumber(int mi, int mx);
void analysis();
void demo();

void leftRotateRB(RBNode **root, RBNode *x)
{
    RBNode *y = x->right;
    p.countOperation(assign, problem_size);
    x->right = y->left;
    p.countOperation(comp, problem_size);
    if(y->left != NIL)
    {
        p.countOperation(assign, problem_size);
        y->left->parent = x;
    }
    p.countOperation(assign, problem_size);
    y->parent = x->parent;
    p.countOperation(comp, problem_size);
    if(x->parent == NIL)
    {
        p.countOperation(assign, problem_size);
        *root = y;
    }
    else
    {
        p.countOperation(comp, problem_size);
        if (x == x->parent->left)
        {
            p.countOperation(assign, problem_size);
            x->parent->left = y;
        }
        else
        {
            p.countOperation(assign, problem_size);
            x->parent->right = y;
        }
    }
    p.countOperation(assign, problem_size, 4);
    y->left = x;
    x->parent = y;
    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;
}

void rightRotateRB(RBNode **root, RBNode *x)
{
    RBNode *y = x->left;
    p.countOperation(assign, problem_size);
    x->left = y->right;
    p.countOperation(comp, problem_size);
    if(y->right != NIL)
    {
        p.countOperation(assign, problem_size);
        y->right->parent = x;
    }
    p.countOperation(assign, problem_size);
    y->parent = x->parent;
    p.countOperation(comp, problem_size);
    if(x->parent == NIL)
    {
        p.countOperation(assign, problem_size);
        *root = y;
    }
    else
    {
        p.countOperation(comp, problem_size);
        if (x == x->parent->left)
        {
            p.countOperation(assign, problem_size);
            x->parent->left = y;
        }
        else
        {
            p.countOperation(assign, problem_size);
            x->parent->right = y;
        }
    }
    p.countOperation(assign, problem_size, 4);
    y->right = x;
    x->parent = y;
    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;
}

void insertRBFix(RBNode **root, RBNode *z)
{
    p.countOperation(comp, problem_size);
    while(z->parent->color == 0)
    {
        p.countOperation(comp, problem_size);
        p.countOperation(comp, problem_size);
        if(z->parent == z->parent->parent->left)
        {
            RBNode *y = z->parent->parent->right;
            p.countOperation(comp, problem_size);
            if(y->color == 0)
            {
                p.countOperation(assign, problem_size, 4);
                z->parent->color = 1;
                y->color = 1;
                z->parent->parent->color = 0;
                z = z->parent->parent;
            }
            else
            {
                p.countOperation(comp, problem_size);
                if(z == z->parent->right)
                {
                    p.countOperation(assign, problem_size);
                    z = z->parent;
                    leftRotateRB(root, z);
                }
                p.countOperation(assign, problem_size, 2);
                z->parent->color = 1;
                z->parent->parent->color = 0;
                rightRotateRB(root, z->parent->parent);
            }
        }
        else
        {
            RBNode *y = z->parent->parent->left;
            p.countOperation(comp, problem_size);
            if(y->color == 0)
            {
                p.countOperation(assign, problem_size, 4);
                z->parent->color = 1;
                y->color = 1;
                z->parent->parent->color = 0;
                z = z->parent->parent;
            }
            else
            {
                p.countOperation(comp, problem_size);
                if(z == z->parent->left)
                {
                    p.countOperation(assign, problem_size);
                    z = z->parent;
                    rightRotateRB(root, z);
                }
                p.countOperation(assign, problem_size, 2);
                z->parent->color = 1;
                z->parent->parent->color = 0;
                leftRotateRB(root, z->parent->parent);
            }
        }
    }
    p.countOperation(assign, problem_size);
    (*root)->color = 1;
}

void insertRB(RBNode **root, int key)
{
    RBNode *y = NIL;
    RBNode *x = *root;
    p.countOperation(comp, problem_size);
    while(x != NIL)
    {
        p.countOperation(comp, problem_size);
        y = x;
        p.countOperation(assign, problem_size);
        y->size++;
        p.countOperation(comp, problem_size);
        p.countOperation(assign, problem_size);
        if(key < x->key)
            x = x->left;
        else
            x = x->right;
    }
    RBNode *z = new RBNode;
    p.countOperation(assign, problem_size, 2);
    z->key = key; //counted
    z->left = NIL;
    z->right = NIL;
    z->parent = y; //counted
    z->color = 0;
    z->size = 1;
    p.countOperation(comp, problem_size);
    if(y == NIL)
    {
        p.countOperation(assign, problem_size);
        *root = z;
    }
    else
    {
        p.countOperation(comp, problem_size);
        p.countOperation(assign, problem_size);
        if (z->key < y->key)
            y->left = z;
        else
            y->right = z;
    }
    insertRBFix(root, z);
}

void transplantRB(RBNode **root, RBNode *u, RBNode *v)
{
    p.countOperation(comp, problem_size);
    if(u->parent == NIL)
    {
        p.countOperation(assign, problem_size);
        *root = v;
    }
    else
    {
        p.countOperation(comp, problem_size);
        p.countOperation(assign, problem_size);
        if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
    }
    p.countOperation(assign, problem_size);
    v->parent = u->parent;
}

RBNode* treeMinRB(RBNode *root)
{
    p.countOperation(comp, problem_size);
    if(root->left != NIL)
        return treeMinRB(root->left);
    return root;
}

void deleteRBFix(RBNode **root, RBNode *x)
{
    p.countOperation(comp, problem_size, 2);
    while(x != *root && x->color == 1)
    {
        p.countOperation(comp, problem_size, 2);
        p.countOperation(comp, problem_size);
        if(x == x->parent->left)
        {
            RBNode *w = x->parent->right;
            p.countOperation(comp, problem_size);
            if(w->color == 0)
            {
                p.countOperation(assign, problem_size, 3);
                w->color = 1;
                x->parent->color = 0;
                leftRotateRB(root, x->parent);
                w = x->parent->right;
            }
            p.countOperation(comp, problem_size);
            if(w->left->color == 1)
                p.countOperation(comp, problem_size);
            if(w->left->color == 1 && w->right->color == 1)
            {
                p.countOperation(assign, problem_size, 2);
                w->color = 0;
                x = x->parent;
            }
            else
            {
                p.countOperation(comp, problem_size);
                if(w->right->color == 1)
                {
                    p.countOperation(assign, problem_size, 3);
                    w->left->color = 1;
                    w->color = 0;
                    rightRotateRB(root, w);
                    w = x->parent->right;
                }
                p.countOperation(assign, problem_size, 4);
                w->color = x->parent->color;
                x->parent->color = 1;
                w->right->color = 1;
                leftRotateRB(root, x->parent);
                x = *root;
            }
        }
        else
        {
            RBNode *w = x->parent->left;
            p.countOperation(comp, problem_size);
            if(w->color == 0)
            {
                p.countOperation(assign, problem_size, 3);
                w->color = 1;
                x->parent->color = 0;
                rightRotateRB(root, x->parent);
                w = x->parent->left;
            }
            p.countOperation(comp, problem_size);
            if(w->left->color == 1)
                p.countOperation(comp, problem_size);
            if(w->left->color == 1 && w->right->color == 1)
            {
                p.countOperation(assign, problem_size, 2);
                w->color = 0;
                x = x->parent;
            }
            else
            {
                p.countOperation(comp, problem_size);
                if(w->left->color == 1)
                {
                    p.countOperation(assign, problem_size, 3);
                    w->right->color = 1;
                    w->color = 0;
                    leftRotateRB(root, w);
                    w = x->parent->left;
                }
                p.countOperation(assign, problem_size, 4);
                w->color = x->parent->color;
                x->parent->color = 1;
                w->left->color = 1;
                rightRotateRB(root, x->parent);
                x = *root;
            }
        }
    }
    p.countOperation(assign, problem_size);
    x->color = 1;
}

void deleteRB(RBNode **root, int key)
{
    if(*root == NIL)
        return;
    RBNode *z = *root;
    p.countOperation(comp, problem_size);
    while(z->key != key)
    {
        p.countOperation(comp, problem_size);
        p.countOperation(comp, problem_size);
        p.countOperation(assign, problem_size);
        if(key < z->key)
            z = z->left;
        else
            z = z->right;
    }
    RBNode *y = z;
    RBNode *x;
    p.countOperation(assign, problem_size);
    bool original = y->color;
    p.countOperation(comp, problem_size);
    if(z->left != NIL)
        p.countOperation(comp, problem_size);
    if(z->left == NIL)
    {
        p.countOperation(assign, problem_size);
        x = z->right;
        transplantRB(root, z, z->right);
        delete z;
    }
    else if(z->right == NIL)
    {
        p.countOperation(assign, problem_size);
        x = z->left;
        transplantRB(root, z, z->left);
        delete z;
    }
    else
    {
        y = treeMinRB(z->right);
        p.countOperation(assign, problem_size, 2);
        original = y->color;
        x = y->right;
        p.countOperation(comp, problem_size);
        if(y->parent == z)
        {
            p.countOperation(assign, problem_size);
            x->parent = y;
        }
        else
        {
            p.countOperation(assign, problem_size, 2);
            transplantRB(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        p.countOperation(assign, problem_size, 4);
        transplantRB(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
        y->size = z->size;
        delete z;
    }
    RBNode *aux = x->parent;
    p.countOperation(comp, problem_size);
    while(aux != NIL)
    {
        p.countOperation(comp, problem_size);
        p.countOperation(assign, problem_size, 2);
        aux->size--;
        aux = aux->parent;
    }
    p.countOperation(comp, problem_size);
    if(original == 1)
        deleteRBFix(root, x);
}

RBNode* buildTreeRB(int *v, int n)
{
    NIL = new RBNode;
    NIL->key = INT32_MIN;
    NIL->size = 0;
    NIL->left = NIL->right = NIL;
    NIL->parent = NIL;
    NIL->color = 1;
    RBNode *root = NIL;
    for(int i = 0;i < n;i++)
        insertRB(&root, v[i]);
    return root;
}

int selectRB(RBNode *root, int i)
{
    if(root == NIL)
        return -1;
    p.countOperation(assign, problem_size);
    int pos = 1;
    p.countOperation(comp, problem_size);
    if(root->left != NIL)
    {
        p.countOperation(assign, problem_size);
        pos += root->left->size;
    }
    p.countOperation(comp, problem_size);
    if(i != pos)
        p.countOperation(comp, problem_size);
    if(i == pos)
        return root->key;
    else if(i < pos)
        return selectRB(root->left, i);
    else
        return selectRB(root->right, i - pos);
}

void prettyPrintRB(RBNode *root, int level)
{
    if(root == NIL)
        return;
    for(int i = 1;i <= level;i++)
        cout<<'\t';
    cout<<root->key<<' '<<root->color<<' '<<root->size<<'\n';
    cout<<"l";
    if(root->left != NIL)
        prettyPrintRB(root->left, level + 1);
    else cout<<'\n';
    cout<<"r";
    if(root->right != NIL)
        prettyPrintRB(root->right, level + 1);
    else
        cout<<'\n';
}

void destroyRB(RBNode *root)
{
    if(root == NIL)
        return;
    if(root->left != nullptr)
        destroyRB(root->left);
    if(root->right != nullptr)
        destroyRB(root->right);
    delete root;
}

BTNode* buildTreeBT(int *v, int i, int j)
{
    p.countOperation(comp, problem_size);
    if(i <= j)
    {
        int pos = (i + j) / 2;
        BTNode *root = new BTNode;
        p.countOperation(assign, problem_size, 4);
        root->key = v[pos];
        root->size = j - i + 1;
        root->left = buildTreeBT(v, i, pos - 1);
        root->right = buildTreeBT(v, pos + 1, j);
        return root;
    }
    return nullptr;
}

BTNode* treeMinBT(BTNode *root)
{
    if(root == nullptr)
        return root;
    p.countOperation(comp, problem_size);
    if(root->left != nullptr)
        return treeMinBT(root->left);
    return root;
}

BTNode* deleteBT(BTNode *root, int key)
{
    if(root == nullptr)
        return root;
    p.countOperation(comp, problem_size);
    if(key >= root->key)
        p.countOperation(comp, problem_size);
    if(key < root->key)
    {
        p.countOperation(assign, problem_size);
        root->left = deleteBT(root->left, key);
    }
    else if(key > root->key)
    {
        p.countOperation(assign, problem_size);
        root->right = deleteBT(root->right, key);
    }
    else
    {
        p.countOperation(comp, problem_size);
        if(root->left != nullptr)
            p.countOperation(comp, problem_size);
        if(root->left != nullptr && root->right != nullptr)
        {
            p.countOperation(assign, problem_size, 2);
            BTNode*successor = treeMinBT(root->right);
            root->key = successor->key;
            root->right = deleteBT(root->right, successor->key);
        }
        else
        {
            BTNode *child;
            p.countOperation(comp, problem_size);
            p.countOperation(assign, problem_size);
            if(root->left != nullptr)
                child = root->left;
            else
                child = root->right;
            delete root;
            p.countOperation(assign, problem_size);
            root = child;
            p.countOperation(comp, problem_size);
            if(child != nullptr)
            {
                p.countOperation(assign, problem_size);
                root->size++;
            }
        }
    }
    p.countOperation(comp, problem_size);
    if(root != nullptr)
    {
        p.countOperation(assign, problem_size);
        root->size--;
    }
    return root;
}

int selectBT(BTNode *root, int i)
{
    if(root == nullptr)
        return -1;
    p.countOperation(assign, problem_size);
    int pos = 1;
    p.countOperation(comp, problem_size);
    if(root->left != nullptr)
    {
        p.countOperation(assign, problem_size);
        pos += root->left->size;
    }
    p.countOperation(comp, problem_size);
    if(i != pos)
        p.countOperation(comp, problem_size);
    if(i == pos)
        return root->key;
    else if(i < pos)
        return selectBT(root->left, i);
    else
        return selectBT(root->right, i - pos);
}

void prettyPrintBT(BTNode *root, int level)
{
    if(root == nullptr)
        return;
    for(int i = 1;i <= level;i++)
        cout<<'\t';
    cout<<root->key<<' '<<root->size<<'\n';
    cout<<"l";
    if(root->left)
        prettyPrintBT(root->left, level + 1);
    else cout<<'\n';
    cout<<"r";
    if(root->right)
        prettyPrintBT(root->right, level + 1);
    else
        cout<<'\n';
}

void destroyBT(BTNode *root)
{
    if(root == nullptr)
        return;
    if(root->left != nullptr)
        destroyBT(root->left);
    if(root->right != nullptr)
        destroyBT(root->right);
    delete root;
}

void printArray(int *v, int n)
{
    for(int i = 0;i < n;i++)
        cout<<v[i]<<' ';
    cout<<'\n';
}

int getRandomNumber(int mi, int mx)
{
    uniform_int_distribution<int> uniform(mi, mx);
    return uniform(engine);
}

void analysis()
{
    cout<<"Analysis"<<'\n';
    p.reset("DynamicOrderStatistics");
    int v[MAX_N], x, i;
    for(int n = MIN_N;n <= MAX_N;n += STEP)
    {
        problem_size = n;
        for(int t = 1;t <= TEST;t++)
        {
            i = n;
            cout<<"Size "<<n<<" Test "<<t<<'\n';
            FillRandomArray(v, n, 1, n, true, ASCENDING);
            strcpy(assign, "BTBuildAssign");
            strcpy(comp, "BTBuildComp");
            BTNode *btRoot = buildTreeBT(v, 0, n - 1);
            for(int j = 1;j <= n;j++)
            {
                strcpy(assign, "BTSelectAssign");
                strcpy(comp, "BTSelectComp");
                x = getRandomNumber(1, i);
                selectBT(btRoot, x);
                strcpy(assign, "BTDeleteAssign");
                strcpy(comp, "BTDeleteComp");
                x = getRandomNumber(0, n - 1);
                while(!v[x])
                    x = getRandomNumber(0, n - 1);
                i--;
                btRoot = deleteBT(btRoot, v[x]);
                v[x] = 0;
            }
            destroyBT(btRoot);
            FillRandomArray(v, n, 1, n, true);
            strcpy(assign, "RBBuildAssign");
            strcpy(comp, "RBBuildComp");
            RBNode *rbRoot = buildTreeRB(v, n);
            i = n;
            for(int j = 1;j <= n;j++)
            {
                strcpy(assign, "RBSelectAssign");
                strcpy(comp, "RBSelectComp");
                x = getRandomNumber(1, i);
                selectRB(rbRoot, x);
                strcpy(assign, "RBDeleteAssign");
                strcpy(comp, "RBDeleteComp");
                x = getRandomNumber(0, n - 1);
                while(!v[x])
                    x = getRandomNumber(0, n - 1);
                i--;
                deleteRB(&rbRoot, v[x]);
                v[x] = 0;
            }
            destroyRB(rbRoot);
            delete NIL;
        }
    }

    p.divideValues("BTBuildAssign", TEST);
    p.divideValues("BTBuildComp", TEST);
    p.divideValues("BTSelectAssign", TEST);
    p.divideValues("BTSelectComp", TEST);
    p.divideValues("BTDeleteAssign", TEST);
    p.divideValues("BTDeleteComp", TEST);

    p.divideValues("RBBuildAssign", TEST);
    p.divideValues("RBBuildComp", TEST);
    p.divideValues("RBSelectAssign", TEST);
    p.divideValues("RBSelectComp", TEST);
    p.divideValues("RBDeleteAssign", TEST);
    p.divideValues("RBDeleteComp", TEST);

    p.addSeries("BTBuildTotal", "BTBuildAssign", "BTBuildComp");
    p.addSeries("BTSelectTotal", "BTSelectAssign", "BTSelectComp");
    p.addSeries("BTDeleteTotal", "BTDeleteAssign", "BTDeleteComp");

    p.addSeries("RBBuildTotal", "RBBuildAssign", "RBBuildComp");
    p.addSeries("RBSelectTotal", "RBSelectAssign", "RBSelectComp");
    p.addSeries("RBDeleteTotal", "RBDeleteAssign", "RBDeleteComp");

    p.createGroup("BuildTotal", "BTBuildTotal", "RBBuildTotal");
    p.createGroup("SelectTotal", "BTSelectTotal", "RBSelectTotal");
    p.createGroup("DeleteTotal", "BTDeleteTotal", "RBDeleteTotal");
    p.showReport();
}

void demo()
{
    int n = 11, x, v[11];
    FillRandomArray(v, n, 1, 11, true, ASCENDING);
    cout<<"Perfectly Balanced BST"<<'\n';
    printArray(v, n);
    BTNode *btRoot = buildTreeBT(v, 0, n - 1);
    prettyPrintBT(btRoot, 0);
    for(int i = 1;i <= n;i++)
        cout<<"Position: "<<i<<" Key: "<<selectBT(btRoot, i)<<'\n';
    while(n)
    {
        x = getRandomNumber(0, 10);
        while(!v[x])
            x = getRandomNumber(0, 10);
        cout<<"Deleting "<<v[x]<<'\n';
        btRoot = deleteBT(btRoot, v[x]);
        n--;
        v[x] = 0;
        cout<<"Result:"<<'\n';
        prettyPrintBT(btRoot, 0);
        if(n > 0)
        {
            x = getRandomNumber(1, n);
            cout << "Selection: Position: " << x << " Key: " << selectBT(btRoot, x) << '\n';
        }
    }
    destroyBT(btRoot);
    n = 11;
    FillRandomArray(v, n, 1, 11, true);
    cout<<"Red Black Tree"<<'\n';
    printArray(v, n);
    RBNode *rbRoot = buildTreeRB(v, n);
    prettyPrintRB(rbRoot, 0);
    for(int i = 1;i <= n;i++)
        cout<<"Position: "<<i<<" Key: "<<selectRB(rbRoot, i)<<'\n';
    while(n)
    {
        x = getRandomNumber(0, 10);
        while(!v[x])
            x = getRandomNumber(0, 10);
        cout<<"Deleting "<<v[x]<<'\n';
        deleteRB(&rbRoot, v[x]);
        n--;
        v[x] = 0;
        cout<<"Result:"<<'\n';
        prettyPrintRB(rbRoot, 0);
        if(n > 0)
        {
            x = getRandomNumber(1, n);
            cout << "Selection: Position: " << x << " Key: " << selectRB(rbRoot, x) << '\n';
        }
    }
    destroyRB(rbRoot);
    delete NIL;
}

int main() {
    demo();
//    analysis();
    return 0;
}

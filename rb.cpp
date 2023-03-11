#include <iostream>
#include <cstring>
#include "rb.h"

using namespace std;

void leftRotateRB(RBNode **root, RBNode *x)
{
    RBNode *y = x->right;
    x->right = y->left;
    if(y->left != NIL)
        y->left->parent = x;
    y->parent = x->parent;
    if(x->parent == NIL)
        *root = y;
    else
    {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;
}

void rightRotateRB(RBNode **root, RBNode *x)
{
    RBNode *y = x->left;
    x->left = y->right;
    if(y->right != NIL)
        y->right->parent = x;
    y->parent = x->parent;
    if(x->parent == NIL)
        *root = y;
    else
    {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;
}

void insertRBFix(RBNode **root, RBNode *z)
{
    while(z->parent->color == 0)
    {
        if(z->parent == z->parent->parent->left)
        {
            RBNode *y = z->parent->parent->right;
            if(y->color == 0)
            {
                z->parent->color = 1;
                y->color = 1;
                z->parent->parent->color = 0;
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->right)
                {
                    z = z->parent;
                    leftRotateRB(root, z);
                }
                z->parent->color = 1;
                z->parent->parent->color = 0;
                rightRotateRB(root, z->parent->parent);
            }
        }
        else
        {
            RBNode *y = z->parent->parent->left;
            if(y->color == 0)
            {
                z->parent->color = 1;
                y->color = 1;
                z->parent->parent->color = 0;
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->left)
                {
                    z = z->parent;
                    rightRotateRB(root, z);
                }
                z->parent->color = 1;
                z->parent->parent->color = 0;
                leftRotateRB(root, z->parent->parent);
            }
        }
    }
    (*root)->color = 1;
}

void insertRB(RBNode **root, int key)
{
    RBNode *y = NIL;
    RBNode *x = *root;
    while(x != NIL)
    {
        y = x;
        y->size++;
        if(key < x->key)
            x = x->left;
        else
            x = x->right;
    }
    RBNode *z = new RBNode;
    z->key = key;
    z->left = NIL;
    z->right = NIL;
    z->parent = y;
    z->color = 0;
    z->size = 1;
    if(y == NIL)
        *root = z;
    else
    {
        if (z->key < y->key)
            y->left = z;
        else
            y->right = z;
    }
    insertRBFix(root, z);
}

void transplantRB(RBNode **root, RBNode *u, RBNode *v)
{
    if(u->parent == NIL)
        *root = v;
    else
    {
        if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
    }
    v->parent = u->parent;
}

RBNode* treeMinRB(RBNode *root)
{
    if(root->left != NIL)
        return treeMinRB(root->left);
    return root;
}

void deleteRBFix(RBNode **root, RBNode *x)
{
    while(x != *root && x->color == 1)
    {
        if(x == x->parent->left)
        {
            RBNode *w = x->parent->right;
            if(w->color == 0)
            {
                w->color = 1;
                x->parent->color = 0;
                leftRotateRB(root, x->parent);
                w = x->parent->right;
            }
            if(w->left->color == 1 && w->right->color == 1)
            {
                w->color = 0;
                x = x->parent;
            }
            else
            {
                if(w->right->color == 1)
                {
                    w->left->color = 1;
                    w->color = 0;
                    rightRotateRB(root, w);
                    w = x->parent->right;
                }
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
            if(w->color == 0)
            {
                w->color = 1;
                x->parent->color = 0;
                rightRotateRB(root, x->parent);
                w = x->parent->left;
            }
            if(w->left->color == 1 && w->right->color == 1)
            {
                w->color = 0;
                x = x->parent;
            }
            else
            {
                if(w->left->color == 1)
                {
                    w->right->color = 1;
                    w->color = 0;
                    leftRotateRB(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 1;
                w->left->color = 1;
                rightRotateRB(root, x->parent);
                x = *root;
            }
        }
    }
    x->color = 1;
}

void deleteRB(RBNode **root, int key)
{
    if(*root == NIL)
        return;
    RBNode *z = *root;
    while(z->key != key)
    {
        if(key < z->key)
            z = z->left;
        else
            z = z->right;
    }
    RBNode *y = z;
    RBNode *x;
    bool original = y->color;
    if(z->left == NIL)
    {
        x = z->right;
        transplantRB(root, z, z->right);
        delete z;
    }
    else if(z->right == NIL)
    {
        x = z->left;
        transplantRB(root, z, z->left);
        delete z;
    }
    else
    {
        y = treeMinRB(z->right);
        original = y->color;
        x = y->right;
        if(y->parent == z)
            x->parent = y;
        else
        {
            transplantRB(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplantRB(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
        y->size = z->size;
        delete z;
    }
    RBNode *aux = x->parent;
    while(aux != NIL)
    {
        aux->size--;
        aux = aux->parent;
    }
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
    int pos = 1;
    if(root->left != NIL)
        pos += root->left->size;
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
    delete NIL;
}
#ifndef RB_H_INCL

#define RB_H_INCL

struct RBNode
{
    int key, size;
    RBNode *parent, *left, *right;
    bool color;
};
RBNode *NIL;

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

#endif
#ifndef TREE_H
#define TREE_H

#include <stdint.h>

typedef union
{
    double  dbl;
    int32_t op;
    char    var;
} NodeValue;

struct Node
{
    int32_t type;

    NodeValue value;

    Node *ancstr;

    Node *left;
    Node *right;
};

void   TreeCtor     (Node *tree);
void   TreeDtor     (Node *tree);

Node  *NodeNew      ();
void   NodeAddChild (Node *node, Node *child);

bool   NodeIsLeaf   (Node *node);

#endif  // TREE_H

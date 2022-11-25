#ifndef TREE_H
#define TREE_H

#include <stdint.h>

typedef union
{
    double      dbl;
    int32_t     op;
    const char *var;
} NodeValue;

struct Node
{
    int32_t type;

    NodeValue value;

    Node *left;
    Node *right;
};

void   TreeCtor     (Node *node);
void   TreeDtor     (Node *node);

void   NodeCtor     (Node *node, int32_t type, NodeValue value, Node *left, Node *right);
Node  *NodeNew      ();
void   NodeAddChild (Node *node, Node *child);
Node  *TreeCopy     (Node *node);

bool   NodeIsLeaf   (Node *node);

#endif  // TREE_H

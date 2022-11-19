#ifndef TREE_H
#define TREE_H

#include <stdint.h>

struct Node
{
    int32_t type;

    union
    {
        double  dbl;
        int32_t op;
        char    var;
    } value;

    Node *ancstr;

    Node *left;
    Node *right;
};

void   TreeCtor(Node *tree);
void   TreeDtor(Node *tree);
Node  *NodeNew();
void NodeAddChild(Node *node, Node *child);
bool NodeIsLeaf(Node *node);

#endif  // TREE_H

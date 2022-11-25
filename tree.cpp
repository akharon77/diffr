#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack_debug.h"
#include "tree.h"
#include "diffr.h"

#define CURR node

void TreeDtor(Node *node)
{
    ASSERT(node != NULL);

    if (LEFT)
        TreeDtor(LEFT);
    if (RIGHT)
        TreeDtor(RIGHT);

    free(CURR);
}

#undef CURR

Node *NodeNew()
{
    Node *res = (Node*) calloc(1, sizeof(Node));
    ASSERT(res != NULL);

    return res;
}

#define CURR node

Node *TreeCopy(Node *node)
{
    Node *node_cpy = NodeNew();
         *node_cpy = *node;

    node_cpy->left  = NULL;
    node_cpy->right = NULL;

    if (LEFT)
        node_cpy->left = TreeCopy(LEFT);
    if (RIGHT)
        node_cpy->right = TreeCopy(RIGHT);

    return node_cpy;
}

void NodeAddChild(Node *node, Node *child)
{
    if (!node->left)
        LEFT  = child;
    else
        RIGHT = child;
}

bool NodeIsLeaf(Node *node)
{
    return LEFT == NULL && RIGHT == NULL;
}

#undef CURR

void   NodeCtor     (Node *node, int32_t type, NodeValue value, Node *left, Node *right)
{
    *node = 
        {
            .type  = type,
            .value = value,
            .left  = left,
            .right = right 
        };
}

Node *CreateNode(int32_t type, NodeValue val, Node *left, Node *right)
{
    Node *node = NodeNew();

    NodeCtor(node, type, val, left, right);

    return node;
}


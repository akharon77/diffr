#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack_debug.h"
#include "tree.h"
#include "diffr.h"

void TreeDtor(Node *vertex)
{
    ASSERT(vertex != NULL);

    if (!NodeIsLeaf(vertex))
    {
        TreeDtor(vertex->left);
        TreeDtor(vertex->right);
    }

    free(vertex);
}

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

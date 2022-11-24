#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack_debug.h"
#include "tree.h"

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

Node *TreeCopy(Node *node)
{
    Node *node_cpy = NodeNew();
    *node_cpy = *node;

    if (!NodeIsLeaf(node))
    {
        node_cpy->left  = TreeCopy(node->left);
        node_cpy->right = TreeCopy(node->right);
    }

    return node_cpy;
}

void NodeAddChild(Node *node, Node *child)
{
    if (!node->left)
        node->left  = child;
    else
        node->right = child;
}

bool NodeIsLeaf(Node *node)
{
    return node->left == NULL && node->right == NULL;
}

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

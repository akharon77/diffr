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

Node *CreateNode(int32_t type, NodeValue val, Node *left, Node *right)
{
    Node *node = NodeNew();

    node->type  = type;
    node->left  = left;
    node->right = right;

    switch (type)
    {
        case TYPE_OP:
            node->value.op = val;
            break;
        case TYPE_VAR:
            node->value.var = val;
            break;
        case TYPE_NUM:
            node->value.num = val;
            break;
        default:
    }

    return node;
}

Node *TreeCopy(Node *node)
{
    Node *node_cpy = NodeNew();

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

    child->ancstr = node;
}

bool NodeIsLeaf(Node *node)
{
    return node->left == NULL && node->right == NULL;
}


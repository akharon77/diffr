#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack_debug.h"
#include "tree.h"

void TreeDtor(Node *vertex)
{
    ASSERT(vertex != NULL);

    free(vertex->str);

    if (!NodeIsLeaf(vertex))
    {
        TreeDtor(vertex->left);
        TreeDtor(vertex->right);
    }

    free(vertex);
}

void NodeCtor(Node *node, const char *str)
{
    ASSERT(node != NULL);

    *node = 
        {
            .str    = strdup(str),
            .ancstr = NULL,
            .left   = NULL,
            .right  = NULL
        };
}

Node *NodeNew()
{
    Node *res = (Node*) calloc(1, sizeof(Node));
    ASSERT(res != NULL);

    *res = 
        {
            .str    = NULL,
            .ancstr = NULL,
            .left   = NULL,
            .right  = NULL
        };

    return res;
}

bool NodeIsLeaf(Node *node)
{
    return node->left == NULL && node->right == NULL;
}

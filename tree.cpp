#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack_debug.h"
#include "tree.h"
#include "diffr.h"

#define CURR node

void TreeDtor(TreeNode *node)
{
    ASSERT(node != NULL);

    if (LEFT)
        TreeDtor(LEFT);
    if (RIGHT)
        TreeDtor(RIGHT);

    free(CURR);
}

TreeNode *TreeNodeNew()
{
    TreeNode *res = (TreeNode*) calloc(1, sizeof(TreeNode));
    ASSERT(res != NULL);

    return res;
}

TreeNode *TreeCopy(TreeNode *node)
{
    TreeNode *node_cpy =  TreeNodeNew();
             *node_cpy = *node;

    node_cpy->left  = NULL;
    node_cpy->right = NULL;

    if (LEFT)
        node_cpy->left = TreeCopy(LEFT);
    if (RIGHT)
        node_cpy->right = TreeCopy(RIGHT);

    return node_cpy;
}

void TreeNodeAddChild(TreeNode *node, TreeNode *child)
{
    if (!node->left)
        LEFT  = child;
    else
        RIGHT = child;
}

bool NodeIsLeaf(TreeNode *node)
{
    return LEFT == NULL && RIGHT == NULL;
}

#undef CURR

void   TreeNodeCtor(TreeNode *node, int32_t type, TreeNodeValue value, TreeNode *left, TreeNode *right)
{
    *node = 
        {
            .type  = type,
            .value = value,
            .size  = (left ? left->size : 0) + (right ? right->size : 0) + 1,
            .left  = left,
            .right = right 
        };
}

TreeNode *CreateTreeNode(int32_t type, TreeNodeValue val, TreeNode *left, TreeNode *right)
{
    TreeNode *node = TreeNodeNew();

    TreeNodeCtor(node, type, val, left, right);

    return node;
}

bool CompareTree(TreeNode *left, TreeNode *right)
{
    if (left == NULL && right == NULL)
        return true;
    else if (left == NULL || right == NULL)
        return false;

    return *left == *right && CompareTree(left->left, right->left) && CompareTree(left->right, right->right);
}


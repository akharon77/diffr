#ifndef TREE_STRUCT_H
#define TREE_STRUCT_H

typedef union
{
    double  dbl;
    int32_t op;
    char    *var;
} TreeNodeValue;

struct TreeNode
{
    int32_t type;

    TreeNodeValue value;

    int32_t size;

    TreeNode *left;
    TreeNode *right;
};

struct LogNode
{
    int32_t type;

    TreeNode *node;
};

#endif  // TREE_STRUCT_H


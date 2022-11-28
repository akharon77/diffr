#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include "tree_struct.h"

void      TreeCtor         (TreeNode *node);
void      TreeDtor         (TreeNode *node);

TreeNode *TreeNodeNew      ();
void      TreeNodeCtor     (TreeNode *node, int32_t type, TreeNodeValue value, TreeNode *left, TreeNode *right);

TreeNode *CreateTreeNode   (int32_t type, TreeNodeValue val, TreeNode *left, TreeNode *right);
TreeNode *TreeCopy         (TreeNode *node);

bool      TreeNodeIsLeaf   (TreeNode *node);
void      TreeNodeAddChild (TreeNode *node, TreeNode *child);
TreeNode *GetMaxSubtree    (TreeNode *node);
int       TreeNodeGetSize  (TreeNode *node);
void      TreeNodeUpdSize  (TreeNode *node);

#endif  // TREE_H


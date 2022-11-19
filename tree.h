#ifndef TREE_H
#define TREE_H

struct Node
{
    char *str;

    Node *ancstr;
    Node *left;
    Node *right;
};

void   TreeCtor(Node *tree);
void   TreeDtor(Node *tree);
Node  *NodeNew();
Node*  TreeInsert(Node *anch, const char *str);
void   NodeCtor(Node *node, const char *str);
bool NodeIsLeaf(Node *node);

#endif  // TREE_H

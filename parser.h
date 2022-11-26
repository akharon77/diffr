#ifndef PARSER_H
#define PARSER_H

#include "tree.h"

const char *GetGeneral    (const char *str, TreeNode *value);
const char *GetExpression (const char *str, TreeNode *value);
const char *GetProduct    (const char *str, TreeNode *value);
const char *GetPower      (const char *str, TreeNode *value);
const char *GetPrimary    (const char *str, TreeNode *value);
const char *GetNumber     (const char *str, TreeNode *value);
const char *GetVariable   (const char *str, TreeNode *value);
const char *GetFunction   (const char *str, TreeNode *value);

#endif  // PARSER_H

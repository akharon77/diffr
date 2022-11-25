#ifndef PARSER_H
#define PARSER_H

#include "tree.h"

const char *GetGeneral    (const char *str, Node *value);
const char *GetExpression (const char *str, Node *value);
const char *GetProduct    (const char *str, Node *value);
const char *GetPower      (const char *str, Node *value);
const char *GetPrimary    (const char *str, Node *value);
const char *GetNumber     (const char *str, Node *value);
const char *GetVariable   (const char *str, Node *value);
const char *GetFunction   (const char *str, Node *value);

#endif  // PARSER_H

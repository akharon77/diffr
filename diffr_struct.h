#ifndef DIFFR_STRUCT_H
#define DIFFR_STRUCT_H

#include "log_struct.h"

enum NODE_TYPES
{
    NODE_TYPE_OP,
    NODE_TYPE_NUM,
    NODE_TYPE_VAR
};

enum OP_CODES
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_SIN,
    OP_COS,
    OP_EXP,
    OP_LN
};

struct Diffr
{
    char     *filename;

    Logger    logger;
    TreeNode *root;

    double    df_x0;
    int32_t   df_n;
};

#endif  // DIFFR_STRUCT_H


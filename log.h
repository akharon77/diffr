#ifndef LOG_H
#define LOG_H

#include "list.h"

enum CONV_TYPES
{
    CONV_TYPE_CONST,
    CONV_TYPE_SOLO_VAR,
    CONV_TYPE_ADD,
    CONV_TYPE_MUL,
    CONV_TYPE_DIV,
    CONV_TYPE_SIN,
    CONV_TYPE_COS,
    CONV_TYPE_EXP_CONST_CONST,
    CONV_TYPE_EXP_CONST_FUNC,
    CONV_TYPE_EXP_FUNC_CONST,
    CONV_TYPE_EXP_FUNC_FUNC,
    CONV_TYPE_LN,
    CONV_TYPES_N
};

struct Conv
{
    int32_t   type;
    TreeNode *node;
};

struct Logger
{
    List convs;
    
    char *filename;
};

void LoggerCtor (Logger *logger, Diffr *diffr);
void LoggerDtor (Logger *logger);

#endif  // LOG_H

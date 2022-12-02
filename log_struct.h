#ifndef LOG_STRUCT_H
#define LOG_STRUCT_H

#include "diffr_struct.h"
#include "tree_struct.h"
#include "stack_struct.h"

const int32_t REPL_SIZE            = 26;
const int32_t MAX_TREE_SIZE        = 30;
const int32_t LOGGER_STK_SIZE_BASE = 64;

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
    CONV_TYPE_SIMP_CONST,
    CONV_TYPE_SIMP_NEUT,
    CONV_TYPE_BEGIN_N_DF,
    CONV_TYPE_RESULT_N_DF,
    CONV_TYPE_BEGIN_TAYLOR,
    CONV_TYPE_RESULT_TAYLOR,
    CONV_TYPE_RESULT,
    CONV_TYPE_REPL,
    CONV_TYPE_EVAL,
    CONV_TYPES_N
};

struct Logger
{
    Stack     convs;

    int32_t   n_repl;
    
    char     *filename;
};

#endif  // LOG_STRUCT_H


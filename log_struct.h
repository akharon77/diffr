#ifndef LOG_STRUCT_H
#define LOG_STRUCT_H

#include "diffr_struct.h"
#include "tree_struct.h"
#include "stack_struct.h"

const int32_t REPL_SIZE            = 26;
const int32_t MAX_TREE_SIZE        = 8;
const int32_t LOGGER_STK_SIZE_BASE = 64;

enum CONV_TYPES
{
    CONV_TYPE_CONST,        // 0
    CONV_TYPE_SOLO_VAR,     // 1
    CONV_TYPE_ADD,          // 2
    CONV_TYPE_MUL,          // 3
    CONV_TYPE_DIV,          // 4
    CONV_TYPE_SIN,          // 5
    CONV_TYPE_COS,          // 6
    CONV_TYPE_EXP_CONST_CONST,//7
    CONV_TYPE_EXP_CONST_FUNC,// 8
    CONV_TYPE_EXP_FUNC_CONST,// 9
    CONV_TYPE_EXP_FUNC_FUNC,// 10
    CONV_TYPE_LN,//11
    CONV_TYPE_SIMP_CONST,//12
    CONV_TYPE_SIMP_NEUT,//13
    CONV_TYPE_RESULT,//14
    CONV_TYPE_REPL,//15
    CONV_TYPES_N//16
};

struct Logger
{
    Stack     convs;

    int32_t   n_repl;
    
    void     *diffr;
    char     *filename;
};

#endif  // LOG_STRUCT_H


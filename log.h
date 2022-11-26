#ifndef LOG_H
#define LOG_H

#include "diffr_struct.h"
#include "log_struct.h"

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
    CONV_TYPE_RESULT,
    CONV_TYPES_N
};

void        LoggerCtor            (Logger *logger, Diffr *diffr);
void        LoggerDtor            (Logger *logger);
void        LoggerLog             (Logger *logger, int32_t type, TreeNode *node);

const char *GetConvDesc           (int32_t type);

void        LoggerPrintToStrLatex (Logger *log, char *str, int32_t id);
char       *PrintToStrLatex       (TreeNode *node, char *str);

#endif  // LOG_H

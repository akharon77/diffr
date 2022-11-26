#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "log.h"
#include "dsl.h"
#include "diffr_debug.h"
#include "tree.h"

const int32_t LOGGER_STK_SIZE_BASE = 64;

void LoggerCtor(Logger *logger, Diffr *diffr)
{
    StackCtor(&logger->convs, LOGGER_STK_SIZE_BASE);  // TODO
    logger->filename = strdup(diffr->filename);
}

void LoggerDtor(Logger *logger)
{
    StackDtor(&logger->convs);
    free(logger->filename);
}

void LoggerLog(Logger *logger, int32_t type, TreeNode *node)
{
    StackPush(&logger->convs, 
              {
                  .type = type, 
                  .node = TreeCopy(node)
              });
}

void LoggerPrintToStrLatex(Logger *log, char *str, int32_t id)
{
    PrintToStrLatex(log->convs.data[id].node, str);
}

#define CURR node

char *PrintToStrLatex(TreeNode *node, char *str)
{
    switch (GET_TYPE(CURR))
    {
        case NODE_TYPE_NUM:
            sprintf(str, "%lf", GET_NUM(CURR));
            break;
        case NODE_TYPE_VAR:
            sprintf(str, "%s", GET_VAR(CURR));
            break;
        case NODE_TYPE_OP:
            {
                if (IS_OP(LEFT) && GetOperatorPriority(GET_OP(LEFT)) < GetOperatorPriority(GET_OP(CURR)))
                    *str++ = '(';
                str = PrintToStrLatex(LEFT, str);
                if (IS_OP(LEFT) && GetOperatorPriority(GET_OP(LEFT)) < GetOperatorPriority(GET_OP(CURR)))
                    *str++ = ')';

                int32_t offset = 0;
                sprintf(str, "%s%n", GetOperatorString(GET_OP(CURR)), &offset);
                str += offset;

                if (IS_OP(RIGHT) && GetOperatorPriority(GET_OP(RIGHT)) < GetOperatorPriority(GET_OP(CURR)))
                    *str++ = '(';
                str = PrintToStrLatex(RIGHT, str);
                if (IS_OP(RIGHT) && GetOperatorPriority(GET_OP(RIGHT)) < GetOperatorPriority(GET_OP(CURR)))
                    *str++ = ')';
            }
        default:
            ASSERT(0);
    }
    return str;
}

const char *GetConvDesc(int32_t type)
{
    switch (type)
    {
        case CONV_TYPE_CONST:
        case CONV_TYPE_EXP_CONST_CONST:
            return "constants";
        case CONV_TYPE_SOLO_VAR:
            return "one-variable";
        case CONV_TYPE_ADD:
            return "sum";
        case CONV_TYPE_MUL:
            return "product";
        case CONV_TYPE_DIV:
            return "dividing";
        case CONV_TYPE_SIN:
            return "sin";
        case CONV_TYPE_COS:
            return "cos";
        case CONV_TYPE_EXP_CONST_FUNC:
            return "exponential function";
        case CONV_TYPE_EXP_FUNC_CONST:
            return "power function";
        case CONV_TYPE_EXP_FUNC_FUNC:
            return "complex exponential function";
    }
}


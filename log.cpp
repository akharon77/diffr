#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "log.h"
#include "dsl.h"
#include "tree.h"
#include "diffr_debug.h"
#include "diffr.h"

void LoggerCtor(Logger *logger, Diffr *diffr)
{
    StackCtor(&logger->convs, LOGGER_STK_SIZE_BASE);  // TODO

    logger->n_repl   = 0;

    logger->diffr    = (void*) diffr;
    logger->filename = strdup(diffr->filename);
}

void LoggerDtor(Logger *logger)
{
    StackDtor(&logger->convs);

    free(logger->filename);
}

#define CURR node

void LoggerLog(Logger *logger, int32_t type, TreeNode *node)
{
    TreeNode *node_cpy = TreeCopy(node);
    printf("\n");

    if (node_cpy->size > MAX_TREE_SIZE)
    {
        TreeNode *max_subtree = GetMaxSubtree(node_cpy);

        node_cpy->size -= max_subtree->size;
        LoggerReplace(logger, max_subtree);

        if (node_cpy->size > MAX_TREE_SIZE)
        {
            max_subtree = GetMaxSubtree(node_cpy);

            node_cpy->size -= max_subtree->size;
            LoggerReplace(logger, max_subtree);
        }
    }

    StackPush(&logger->convs, 
              {
                  .type = type, 
                  .node = node_cpy
              });

    TreeNode *buf = ((Diffr*) logger->diffr)->root;
    ((Diffr*) logger->diffr)->root = node_cpy;
    DiffrDump((Diffr*) logger->diffr);
    ((Diffr*) logger->diffr)->root = buf;
}

void LoggerPrintToStrLatex(Logger *log, char *str, int32_t id)
{
    PrintToStrLatex(log->convs.data[id].node, str);
}

#define WRAP_PRINT(node)                                                                            \
do                                                                                                  \
{                                                                                                   \
    if (node)                                                                                       \
    {                                                                                               \
        if (IS_OP(node) && GetOperatorPriority(GET_OP(node)) < GetOperatorPriority(GET_OP(CURR)))   \
        {                                                                                           \
            sprintf(str, "\\left(");                                                                \
            str += 6;                                                                               \
        }                                                                                           \
                                                                                                    \
        str = PrintToStrLatex(node, str);                                                           \
                                                                                                    \
        if (IS_OP(node) && GetOperatorPriority(GET_OP(node)) < GetOperatorPriority(GET_OP(CURR)))   \
        {                                                                                           \
            sprintf(str, "\\right)");                                                               \
            str += 7;                                                                               \
        }                                                                                           \
    }                                                                                               \
}                                                                                                   \
while (0)

char *PrintToStrLatex(TreeNode *node, char *str)
{
    int32_t offset = 0;

    sprintf(str, "{");
    ++str;

    switch (GET_TYPE(CURR))
    {
        case NODE_TYPE_NUM:
            {
                if (GET_NUM(CURR) < -EPS)
                {
                    sprintf(str, "(");
                    ++str;
                }

                sprintf(str, "%lg%n", GET_NUM(CURR), &offset);
                str += offset;

                if (GET_NUM(CURR) < -EPS)
                {
                    sprintf(str, ")");
                    ++str;
                }
            }
            break;
        case NODE_TYPE_VAR:
            {
                sprintf(str, "%s%n", GET_VAR(CURR), &offset);
                str += offset;
            }
            break;
        case NODE_TYPE_OP:
            {
                if (GET_OP(CURR) == OP_DIV)
                {
                    sprintf(str, "\\cfrac");
                    str += 6;
                }

                WRAP_PRINT(LEFT);

                sprintf(str, "%s%n", GetOperatorStringLatex(GET_OP(CURR)), &offset);
                str += offset;

                WRAP_PRINT(RIGHT);
            }
            break;
        default:
            ASSERT(0);
    }

    sprintf(str, "}");
    ++str;

    return str;
}

#undef CURR

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

const char *GetGreekAlphabet(int32_t id)
{
    static const char* const latex_greek[] =
        {
            "\\alpha",
            "\\beta",
            "\\gamma",
            "\\delta",
            "\\varepsilon",
            "\\zeta",
            "\\eta",
            "\\theta",
            "\\iota",
            "\\kappa",
            "\\varkappa"
            "\\lambda",
            "\\mu",
            "\\nu",
            "\\delta",
            "\\rho",
            "\\varphi",
            "\\xi",
            "\\o",
            "\\chi",
            "\\psi",
            "\\omega",
            "\\Omega",
            "\\Psi",
            "\\Xi",
            "\\O",
            "\\Delta",
            "\\Mu",
            "\\Nu",
            "\\Rho"
        };

    static char res_var[512] = "";

    sprintf(res_var, "{%s}_{%d}", latex_greek[id % 30], id / 30);
    return res_var;
}

void LoggerReplace(Logger *logger, TreeNode *node)
{
    LoggerLog(logger, CONV_TYPE_REPL, node);

    TreeDtor(node->left);
    TreeDtor(node->right);
    VAR_CTOR(node, GetGreekAlphabet(logger->n_repl++));
    
    LoggerLog(logger, CONV_TYPE_RESULT, node);
}


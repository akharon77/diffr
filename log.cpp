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

    // TreeNode *buf = ((Diffr*) logger->diffr)->root;
    // ((Diffr*) logger->diffr)->root = node_cpy;
    // DiffrDump((Diffr*) logger->diffr);
    // ((Diffr*) logger->diffr)->root = buf;
}

void LoggerGenerateToFdLatexBook(Logger *logger, int32_t fd)
{
    dprintf(fd, "\\documentclass[12pt, letterpaper]{book}\n"

                "\\usepackage[utf8]{inputenc}\n"
                "\\usepackage{amsmath}\n"
                "\\usepackage{amsfonts}\n"
                "\\usepackage{mathtools}\n"
                "\\usepackage{amssymb}\n"
                "\\usepackage{extarrows}\n"

                "\\usepackage{graphicx}\n"
                "\\usepackage{graphics}\n"
                "\\usepackage{fancyhdr}\n"

                "\\pagestyle{fancy}\n"

                "\\begin{document}\n"

                "\\title{Differentiating functions}\n"
                "\\author{Abdullin Timur Arturovich}\n"

                "\\maketitle\n"

                "\\tableofcontents\n");

    for (int32_t i = 0; i < logger->convs.size; ++i)
        LoggerPrintToFdLatex(logger, fd, i);

    dprintf(fd, "\\end{document}\n");
}

const char *GetRandomComment()
{
    return "Ba!";
}

void LoggerPrintToFdLatex(Logger *logger, int32_t fd, int32_t id)
{
    dprintf(fd, "\\textbf{\n");
    dprintf(fd, "%s\n", GetRandomComment());

    switch (logger->convs.data[id].type)
    {
        case CONV_TYPE_REPL:
            dprintf(fd, "Let's introduce the replacement of the expression with a variable");
            break;
        case CONV_TYPE_RESULT:
            dprintf(fd, "We got this result");
            break;
        case CONV_TYPE_SIMP_CONST:
        case CONV_TYPE_SIMP_NEUT:
            dprintf(fd, "Let's try to simplify");
            break;
        default:
            dprintf(fd, "Let's  use the rule of differentiating %s", GetConvDesc(logger->convs.data[id].type));
    }
    dprintf(fd, ":\n}\n");

    dprintf(fd, "$");
    PrintToFdLatex(logger->convs.data[id].node, fd);
    dprintf(fd, "$\n");
}

#define WRAP_PRINT(node)                                                                            \
do                                                                                                  \
{                                                                                                   \
    if (node)                                                                                       \
    {                                                                                               \
        if (IS_OP(node) && GetOperatorPriority(GET_OP(node)) < GetOperatorPriority(GET_OP(CURR)))   \
            dprintf(fd, "\\left(");                                                                 \
                                                                                                    \
        PrintToFdLatex(node, fd);                                                                   \
                                                                                                    \
        if (IS_OP(node) && GetOperatorPriority(GET_OP(node)) < GetOperatorPriority(GET_OP(CURR)))   \
            dprintf(fd, "\\right)");                                                                \
    }                                                                                               \
}                                                                                                   \
while (0)

void PrintToFdLatex(TreeNode *node, int32_t fd)
{
    dprintf(fd, "{");

    switch (GET_TYPE(CURR))
    {
        case NODE_TYPE_NUM:
            {
                if (GET_NUM(CURR) < -EPS)
                    dprintf(fd, "(");

                dprintf(fd, "%lg", GET_NUM(CURR));

                if (GET_NUM(CURR) < -EPS)
                    dprintf(fd, ")");
            }
            break;
        case NODE_TYPE_VAR:
            dprintf(fd, "%s", GET_VAR(CURR));
            break;
        case NODE_TYPE_OP:
            {
                if (GET_OP(CURR) == OP_DIV)
                    dprintf(fd, "\\cfrac");

                WRAP_PRINT(LEFT);

                dprintf(fd, "%s", GetOperatorStringLatex(GET_OP(CURR)));

                WRAP_PRINT(RIGHT);
            }
            break;
        default:
            ASSERT(0);
    }

    dprintf(fd, "}");
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


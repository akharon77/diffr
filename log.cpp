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
    for (int32_t i = 0; i < logger->convs.size; ++i)
        TreeDtor(logger->convs.data[i].node);

    StackDtor(&logger->convs);

    free(logger->filename);
}

#define CURR node

void LoggerLog(Logger *logger, int32_t type, TreeNode *node)
{
    TreeNode *node_cpy = TreeCopy(node);

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

    // TreeDump(node_cpy, "log");

    StackPush(&logger->convs, 
              {
                  .type = type, 
                  .node = node_cpy
              });
}

void DiffrGenerateFdLatexBook(Diffr *diffr, int32_t fd)
{
    Logger *logger = &diffr->logger;

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

                "\\tableofcontents\n"

                "\\newpage\n"
                );

    for (int32_t i = 0; i < logger->convs.size; ++i)
        LoggerPrintToFdLatex(logger, fd, i);

    dprintf(fd, "\\chapter{Graph}\n");

    PrintGraphToFile(diffr->root, diffr->tangent, "func_graph.gpi");

    dprintf(fd, "\\includegraphics[width=\\textwidth]{./func_graph.png}\n");

    dprintf(fd, "\\end{document}\n");
}

void PrintGraphToFile(TreeNode *func, TreeNode *tangent, const char *filename)
{
    int32_t fd_plot = creat(filename, S_IRWXU);

    dprintf(fd_plot,
            "set xlabel \"X\"\n"
            "set ylabel \"Y\"\n"
            "set grid\n"
           );


    dprintf(fd_plot, 
            "\n"
            "set samples 10000\n"
            "set terminal png size 800, 600\n"
            "set output \"func_graph.png\"\n"
            "plot "
            );

    PrintToFdPlot(func, fd_plot);
    dprintf(fd_plot, " title \"func\" lc rgb \"red\", ");
    PrintToFdPlot(tangent, fd_plot);
    dprintf(fd_plot, " title \"tang\" lc rgb \"green\"");

    close(fd_plot);

    char cmd[512] = "";
    sprintf(cmd, "gnuplot %s", filename);
    system(cmd);
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
        case CONV_TYPE_BEGIN_TANGENT:
            dprintf(fd, 
                    "\\LARGE\n"
                    "\\chapter{Tangent}\n"
                    "\\normalsize\n"
                    "Let's find tangent of this function");
            break;
        case CONV_TYPE_RESULT_TANGENT:
            dprintf(fd, "We got tangent");
            break;
        case CONV_TYPE_EVAL:
            dprintf(fd, "Let's find value of this expression");
            break;
        case CONV_TYPE_BEGIN_N_DF:
            dprintf(fd, 
                    "\\LARGE\n"
                    "\\section{$\\frac{df}{dx}$}\n"
                    "\\normalsize\n"
                    "Let's find the next derivative of the function"
                    );
            break;
        case CONV_TYPE_BEGIN_TAYLOR:
            dprintf(fd, 
                    "\\LARGE\n"
                    "\\chapter{Taylor series}"
                    "\\normalsize\n"
                    "Let's find Taylor series expansion");
            break;
        case CONV_TYPE_RESULT_N_DF:
            dprintf(fd, "We got the next derivative of the function");
            break;
        case CONV_TYPE_RESULT_TAYLOR:
            dprintf(fd, "We got Taylor series expansion");
            break;
        case CONV_TYPE_REPL:
            dprintf(fd, "Let's introduce the replacement of the expression with a variable");
            break;
        case CONV_TYPE_RESULT:
            dprintf(fd, "We got this result");
            break;
        case CONV_TYPE_SIMP_CONST:
            dprintf(fd, "Let's try to simplify constants");
            break;
        case CONV_TYPE_SIMP_NEUT:
            dprintf(fd, "Let's try to simplify neutral elements");
            break;
        default:
            dprintf(fd, "Let's use the rule of differentiating %s", GetConvDesc(logger->convs.data[id].type));
    }
    dprintf(fd, ":\n}\n\n");

    dprintf(fd, "\\begin{center}\n$");
    PrintToFdLatex(logger->convs.data[id].node, fd);
    dprintf(fd, "$\n\\end{center}\n\n");
}

#define WRAP_PRINT(node)                                                                            \
do                                                                                                  \
{                                                                                                   \
    if (node)                                                                                       \
    {                                                                                               \
        if (IS_OP(node) && GetOperatorPriority(GET_OP(node)) < GetOperatorPriority(GET_OP(CURR)))   \
            dprintf(fd, "{\\left(");                                                                \
                                                                                                    \
        PrintToFdLatex(node, fd);                                                                   \
                                                                                                    \
        if (IS_OP(node) && GetOperatorPriority(GET_OP(node)) < GetOperatorPriority(GET_OP(CURR)))   \
            dprintf(fd, "\\right)}");                                                               \
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

                dprintf(fd, "%lf", GET_NUM(CURR));

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

#undef WRAP_PRINT
#define WRAP_PRINT(node)                                                                            \
do                                                                                                  \
{                                                                                                   \
    if (node)                                                                                       \
    {                                                                                               \
        if (IS_OP(node) && GetOperatorPriority(GET_OP(node)) < GetOperatorPriority(GET_OP(CURR)))   \
            dprintf(fd, "(");                                                                       \
                                                                                                    \
        PrintToFdPlot(node, fd);                                                                    \
                                                                                                    \
        if (IS_OP(node) && GetOperatorPriority(GET_OP(node)) < GetOperatorPriority(GET_OP(CURR)))   \
            dprintf(fd, ")");                                                                       \
    }                                                                                               \
}                                                                                                   \
while (0)

void PrintToFdPlot(TreeNode *node, int32_t fd)
{
    switch (GET_TYPE(CURR))
    {
        case NODE_TYPE_NUM:
            dprintf(fd, "%lf", GET_NUM(CURR));
            break;
        case NODE_TYPE_VAR:
            dprintf(fd, "%s", GET_VAR(CURR));
            break;
        case NODE_TYPE_OP:
            {
                WRAP_PRINT(LEFT);
                dprintf(fd, "%s", GetOperatorStringPlot(GET_OP(CURR)));

                if (IS_OP_CODE(CURR, OP_SIN) || IS_OP_CODE(CURR, OP_COS))
                    dprintf(fd, "(");

                WRAP_PRINT(RIGHT);

                if (IS_OP_CODE(CURR, OP_SIN) || IS_OP_CODE(CURR, OP_COS))
                    dprintf(fd, ")");
            }
            break;
    }
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
        case CONV_TYPE_LN:
            return "logarithmic function";
        default:
            return "(null)";
    }
}

const char *GetGreekAlphabet(int32_t id)
{
    static const char* latex_greek[] =
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
            "\\Delta"
        };

    static char res_var[512] = "";

    sprintf(res_var, "{%s}_{%d}", latex_greek[id % 26], id / 26);
    return res_var;
}

void LoggerReplace(Logger *logger, TreeNode *node)
{
    LoggerLog(logger, CONV_TYPE_REPL, node);

    TreeNode *node_subst = TreeCopy(node);

    if (LEFT)
        TreeDtor(node->left);

    if (RIGHT)
        TreeDtor(node->right);

    VAR_CTOR(node, GetGreekAlphabet(logger->n_repl++), node_subst);
    
    LoggerLog(logger, CONV_TYPE_RESULT, node);
}

const char *GetOperatorStringPlot(int32_t op_code)
{
    switch (op_code)
    {
        case OP_ADD:
            return "+";
        case OP_SUB:
            return "-";
        case OP_MUL:
            return "*";
        case OP_DIV:
            return "/";
        case OP_SIN:
            return "sin";
        case OP_COS:
            return "cos";
        case OP_EXP:
            return "**";
        case OP_LN:
            return "log";
        default:
            return "(null)";
    }
}

const char *GetOperatorStringLatex(int32_t op_code)
{
    switch (op_code)
    {
        case OP_ADD:
            return "+";
        case OP_SUB:
            return "-";
        case OP_MUL:
            return "\\cdot";
        case OP_SIN:
            return "\\sin";
        case OP_COS:
            return "\\cos";
        case OP_EXP:
            return "^";
        case OP_LN:
            return "\\ln";
        default:
            return "";
    }
}

#undef CURR


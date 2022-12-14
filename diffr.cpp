#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "diffr.h"
#include "diffr_debug.h"
#include "tree.h"
#include "iostr.h"
#include "stack.h"
#include "colors.h"
#include "parser.h"
#include "dsl.h"
#include "log.h"

const Option EXEC_OPTIONS[] = 
    {
        {"--file",     "-f",  FILENAME_OPTION, "from file"},
        {"--df",       "-d",  DF_OPTION,       "df"},
        {"--dfn",      "-n",  DF_N_OPTION,     "n"},
        {"--dfx0",     "-x",  DF_X0_OPTION,    "x0"},
        {"--help",     "-h",  HELP_OPTION,     "show help"}
    };
 
const size_t N_EXEC_OPTIONS = sizeof(EXEC_OPTIONS) / sizeof(Option);

void DiffrCtor(Diffr *diffr, double df_x0, int32_t df_n)
{
    ASSERT(diffr != NULL);

    diffr->df_x0    = df_x0;
    diffr->df_n     = df_n;
    diffr->root     = NULL;
    diffr->tangent  = NULL;
    diffr->filename = NULL;
    diffr->logger   = {};
}

void DiffrDtor(Diffr *diffr)
{
    ASSERT(diffr != NULL);

    TreeDtor(diffr->root);
    TreeDtor(diffr->tangent);
    LoggerDtor(&diffr->logger);
    free(diffr->filename);
}

void DiffrRun(Diffr *diffr)
{
    TaylorSeries(diffr->root, diffr->df_x0, diffr->df_n, &diffr->logger);
    diffr->tangent = Tangent(diffr->root, diffr->df_x0, &diffr->logger);
    
    char cmd            [1024] = "";
    char latex_filename [512]  = "";

    sprintf(latex_filename,    "%s_book.tex", diffr->filename);
    sprintf(cmd,            "rm %s_book.pdf", diffr->filename);
    system(cmd);

    int32_t fd = creat(latex_filename, S_IRWXU);

    DiffrGenerateFdLatexBook(diffr, fd);

    close(fd);

    sprintf(cmd, "/mnt/c/Users/Timur/AppData/Local/Programs/MiKTeX/miktex/bin/x64/pdflatex.exe %s", latex_filename);
    system(cmd);
}

void DiffrInput(Diffr *diffr, const char *filename, int32_t *err)
{
    TextInfo text = {};
    TextInfoCtor(&text);

    InputText(&text, filename, err);
    InitTextSep(&text);

    diffr->root = TreeNodeNew();
    GetGeneral(text.base, diffr->root);

    diffr->filename = strdup(filename);

    LoggerCtor(&diffr->logger, diffr);

    TextInfoDtor(&text);
}

#define LGR logger
#define CURR node

TreeNode* Differentiate(TreeNode *node, Logger *logger)
{
    ASSERT(node != NULL);

    TreeNode *result = NULL;

    switch (node->type)
    {
        case NODE_TYPE_NUM: 
            {
                LoggerLog(logger, CONV_TYPE_CONST, CURR);
                result = CREATE_NUM(0);
            }
            break;
        case NODE_TYPE_VAR:
            {
                if (strcasecmp(GET_VAR(CURR), "x"))
                    break;  // TODO: select variable in diffr

                LoggerLog(logger, CONV_TYPE_SOLO_VAR, CURR);
                result = CREATE_NUM(1);
            }
            break;
        case NODE_TYPE_OP:
            switch (GET_OP(CURR))
            {
                case OP_ADD: 
                    {
                        LoggerLog(logger, CONV_TYPE_ADD, CURR);
                        result = ADD(D_L, D_R);
                    }
                break;
                case OP_SUB:
                    {
                        LoggerLog(logger, CONV_TYPE_ADD, CURR);
                        result = SUB(D_L, D_R);
                    }
                break;

                case OP_MUL:
                    {
                        LoggerLog(logger, CONV_TYPE_MUL, CURR);
                        result = ADD(MUL(D_L, CP_R), MUL(CP_L, D_R));
                    }
                break;
                case OP_DIV:
                    {
                        LoggerLog(logger, CONV_TYPE_DIV, CURR);
                        result = 
                            DIV
                            (
                                SUB
                                (
                                    MUL(D_L, CP_R), 
                                    MUL(CP_L, D_R)
                                ),
                                EXP(CP_R, CREATE_NUM(2))
                            );
                    }
                break;

                case OP_SIN:
                    {
                        LoggerLog(logger, CONV_TYPE_SIN, CURR);
                        result = MUL(COS(CP_R), D_R);
                    }
                break;
                case OP_COS:
                    {
                        LoggerLog(logger, CONV_TYPE_COS, CURR);
                        result = MUL(CREATE_NUM(-1), MUL(SIN(CP_R), D_R));
                    }
                break;

                case OP_EXP:
                    if (IS_NUM(LEFT) && IS_NUM(RIGHT))
                    {
                        LoggerLog(logger, CONV_TYPE_EXP_CONST_CONST, CURR);
                        result = CREATE_NUM(0);
                    }
                    else if (IS_NUM(LEFT) && IS_FUNC(RIGHT))
                    {
                        LoggerLog(logger, CONV_TYPE_EXP_CONST_FUNC, CURR);
                        result = MUL(EXP(CP_L, CP_R), MUL(LN(CP_L), D_R));
                    }
                    else if (IS_FUNC(LEFT) && IS_NUM(RIGHT))
                    {
                        LoggerLog(logger, CONV_TYPE_EXP_FUNC_CONST, CURR);
                        result = MUL(CP_R, MUL(EXP(CP_L, CREATE_NUM(GET_NUM(RIGHT) - 1)), D_L));
                    }
                    else
                        {
                            LoggerLog(logger, CONV_TYPE_EXP_FUNC_FUNC, CURR);
                            TreeNode *fict_node = EXP(CREATE_NUM(exp(1)), MUL(LN(CP_L), CP_R));  // TODO: e const
                            TreeNode *res = Differentiate(fict_node, logger);
                            TreeDtor(fict_node);

                            result = res;
                        }
                break;
                
                case OP_LN:
                    {
                        LoggerLog(logger, CONV_TYPE_LN, CURR);
                        result = MUL(DIV(CREATE_NUM(1), CP_R), D_R);
                    }
                break;
            }
        break;
        default:
            ASSERT(0);
    }

    Simplify(result, logger);

    LoggerLog(logger, CONV_TYPE_RESULT, result);
    return result;
}

#undef DFR

void Simplify(TreeNode *node, Logger *logger)
{
    if (LEFT)
        Simplify(LEFT,  logger);
    if (RIGHT)
        Simplify(RIGHT, logger);

    Rotate            (CURR);

    SimplifyConst     (CURR, logger);
    SimplifyNeutral   (CURR, logger);
}

void SimplifyConst(TreeNode *node, Logger *logger)
{
    if (IS_VAR(CURR) || IS_NUM(CURR))
        return;
    
    LoggerLog(logger, CONV_TYPE_SIMP_CONST, CURR);

    switch (GET_OP(CURR))
    {
        case OP_ADD:
            {
                if (IS_NUM(LEFT) && IS_NUM(RIGHT))
                {
                    GET_NUM(CURR) = GET_NUM(LEFT) + GET_NUM(RIGHT);
                    GET_TYPE(CURR) = NODE_TYPE_NUM;
                    
                    free(LEFT);
                    LEFT = NULL;

                    free(RIGHT);
                    RIGHT = NULL;
                }
                else if (IS_NUM(LEFT)               && 
                        (IS_OP_CODE(RIGHT, OP_ADD)  ||
                         IS_OP_CODE(RIGHT, OP_SUB)) && 
                         IS_NUM(RIGHT->left))
                {
                    TreeNode *last_right = RIGHT;
                    GET_NUM(LEFT) = GET_NUM(LEFT) + GET_NUM(RIGHT->left);
                    GET_OP(CURR)  = GET_OP(RIGHT);

                    free(RIGHT->left);
                    RIGHT = RIGHT->right;

                    free(last_right);
                }
            }
            break;
        case OP_SUB:
            {
                if (IS_NUM(LEFT) && IS_NUM(RIGHT))
                {
                    GET_NUM(CURR) = GET_NUM(LEFT) - GET_NUM(RIGHT);
                    GET_TYPE(CURR) = NODE_TYPE_NUM;

                    free(LEFT);
                    LEFT = NULL;

                    free(RIGHT);
                    RIGHT = NULL;
                }
                else if (IS_NUM(LEFT)               && 
                        (IS_OP_CODE(RIGHT, OP_ADD)  ||
                         IS_OP_CODE(RIGHT, OP_SUB)) && 
                         IS_NUM(RIGHT->left))
                {
                    TreeNode *last_right = RIGHT;
                    GET_NUM(LEFT) = GET_NUM(LEFT) - GET_NUM(RIGHT->left);
                    GET_OP(CURR) = IS_OP_CODE(RIGHT, OP_ADD) ? OP_SUB : OP_ADD;

                    free(RIGHT->left);
                    RIGHT = RIGHT->right;

                    free(last_right);
                }
            }
            break;
        case OP_MUL:
            if (IS_NUM(LEFT) && IS_NUM(RIGHT))
            {
                GET_NUM(CURR) = GET_NUM(LEFT) * GET_NUM(RIGHT);
                GET_TYPE(CURR) = NODE_TYPE_NUM;

                free(LEFT);
                LEFT = NULL;

                free(RIGHT);
                RIGHT = NULL;
            }
            else if (IS_NUM(LEFT)              &&
                     IS_OP_CODE(RIGHT, OP_MUL) &&
                     IS_NUM(RIGHT->left))
            {
                TreeNode *last_right = RIGHT;
                GET_NUM(LEFT) = GET_NUM(LEFT) * GET_NUM(RIGHT->left);

                free(RIGHT->left);
                RIGHT = RIGHT->right;

                free(last_right);
            }
            break;
        case OP_DIV:
            if (IS_NUM(LEFT) && IS_NUM(RIGHT))
            {
                GET_NUM(CURR) = GET_NUM(LEFT) / GET_NUM(RIGHT);
                GET_TYPE(CURR) = NODE_TYPE_NUM;

                free(LEFT);
                LEFT = NULL;

                free(RIGHT);
                RIGHT = NULL;
            }
            break;
        case OP_SIN:
            if (IS_NUM(RIGHT))
            {
                GET_NUM(CURR) = sin(GET_NUM(RIGHT));
                GET_TYPE(CURR) = NODE_TYPE_NUM;

                free(RIGHT);
                RIGHT = NULL;
            }
            break;
        case OP_COS:
            if (IS_NUM(RIGHT))
            {
                GET_NUM(CURR) = cos(GET_NUM(RIGHT));
                GET_TYPE(CURR) = NODE_TYPE_NUM;

                free(RIGHT);
                RIGHT = NULL;
            }
            break;
        case OP_EXP:
            if (IS_NUM(LEFT) && IS_NUM(RIGHT))
            {
                GET_NUM(CURR) = pow(GET_NUM(LEFT), GET_NUM(RIGHT));
                GET_TYPE(CURR) = NODE_TYPE_NUM;

                free(LEFT);
                LEFT = NULL;

                free(RIGHT);
                RIGHT = NULL;
            }
            break;
        case OP_LN:
            if (IS_NUM(RIGHT))
            {
                GET_NUM(CURR) = log(GET_NUM(RIGHT));
                GET_TYPE(CURR) = NODE_TYPE_NUM;

                free(RIGHT);
                RIGHT = NULL;
            }
            break;
        default:
            assert(0 && "Wrong operation");
    }
    
    TreeNodeUpdSize(CURR);

    LoggerLog(logger, CONV_TYPE_RESULT, CURR);
}

void SimplifyNeutral(TreeNode *node, Logger *logger)
{
    if (IS_NUM(CURR) || IS_VAR(CURR))
        return;

    LoggerLog(logger, CONV_TYPE_SIMP_NEUT, CURR);

    switch (GET_OP(CURR))
    {
        case OP_ADD:
            if (IS_ZERO(LEFT))
            {
                free(LEFT);

                TreeNode *last_right =  RIGHT;
                         *CURR       = *RIGHT;

                free(last_right);
            }
            break;
        case OP_SUB:
            if (IS_ZERO(RIGHT))
            {
                free(RIGHT);

                TreeNode *last_left =  LEFT;
                         *CURR      = *LEFT;

                free(last_left);
            }
            else if (IS_ZERO(LEFT))
            {
                free(LEFT);

                OP_CTOR(CURR, OP_MUL, CREATE_NUM(-1), RIGHT);
            }
        case OP_MUL:
            if (IS_ZERO(LEFT))
            {
                free(LEFT);
                TreeDtor(RIGHT);

                NUM_CTOR(CURR, 0);
            }
            else if (IS_ONE(LEFT))
            {
                free(LEFT);

                TreeNode *last_right =  RIGHT;
                     *CURR       = *RIGHT;

                free(last_right);
            }
            break;
        case OP_DIV:
            if (IS_ZERO(LEFT))
            {
                free(LEFT);
                TreeDtor(RIGHT);

                NUM_CTOR(CURR, 0);
            }
            else if (IS_ONE(RIGHT))
            {
                free(RIGHT);

                TreeNode *last_left =  LEFT;
                         *CURR      = *LEFT;

                free(last_left);
            }
            break;
        case OP_EXP:
            if (IS_ZERO(LEFT))
            {
                free(LEFT);
                TreeDtor(RIGHT);

                NUM_CTOR(CURR, 0);
            }
            else if (IS_ONE(LEFT) || IS_ZERO(RIGHT))
            {
                TreeDtor(LEFT);
                TreeDtor(RIGHT);

                NUM_CTOR(CURR, 1);
            }
            else if (IS_ONE(RIGHT))
            {
                free(RIGHT);

                TreeNode *last_left =  LEFT;
                     *CURR      = *LEFT;
                
                free(last_left);
            }
    }

    TreeNodeUpdSize(CURR);

    LoggerLog(logger, CONV_TYPE_RESULT, CURR);
}

void Rotate(TreeNode *node)
{
    RotateCommutative (node);
}

void RotateCommutative(TreeNode *node)
{
    if ((IS_OP_CODE(CURR, OP_ADD) || IS_OP_CODE(CURR, OP_MUL)) && IS_NUM(RIGHT))
    {
        TreeNode *buf   = LEFT;
                  LEFT  = RIGHT;
                  RIGHT = buf;
    }
}

TreeNode *TaylorSeries(TreeNode *node, double x0, int32_t n, Logger *logger)
{
    LoggerLog(logger, CONV_TYPE_BEGIN_TAYLOR, CURR);

    TreeNode *result = CREATE_NUM(Evaluate(CURR, x0, logger));
    TreeNode *df     = TreeCopy(CURR);
    double    fac    = 1;

    for (int32_t i = 1; i < n + 1; ++i)
    {
        LoggerLog(logger, CONV_TYPE_BEGIN_N_DF, df);

        TreeNode *next_df = Differentiate(df, logger);
        fac    *= i;

        TreeDtor(df);
        df      = next_df;

        TreeNode *mid_res = ADD(TreeCopy(result), MUL(DIV(CREATE_NUM(Evaluate(df, x0, logger)), CREATE_NUM(fac)), EXP(SUB(CREATE_VAR("x"), CREATE_NUM(x0)), CREATE_NUM(i))));

        TreeDtor(result);
        result  = mid_res;

        Simplify(result, logger);

        LoggerLog(logger, CONV_TYPE_RESULT_N_DF, df);

        TreeDump(df, "df");
    }

    TreeDtor(df);

    LoggerLog(logger, CONV_TYPE_RESULT_TAYLOR, result);

    return result;
}

double Evaluate(TreeNode *node, double x, Logger *logger)
{
    double lhs = 0;
    double rhs = 0;
    double res = 0;

    LoggerLog(logger, CONV_TYPE_EVAL, node);

    if (LEFT)
        lhs = Evaluate(LEFT,  x, logger);
    if (RIGHT)
        rhs = Evaluate(RIGHT, x, logger);

    switch (GET_TYPE(CURR))
    {
        case NODE_TYPE_NUM:
            res = GET_NUM(CURR);
            break;
        case NODE_TYPE_VAR:
            {
                if (strcasecmp(GET_VAR(CURR), "x") == 0)
                    res = x;
                else
                    res = Evaluate(RIGHT, x, logger);
            }
            break;
        case NODE_TYPE_OP:
            switch (GET_OP(CURR))
            {
                case OP_ADD:
                    res = lhs + rhs;
                    break;
                case OP_SUB:
                    res = lhs - rhs;
                    break;
                case OP_MUL:
                    res = lhs * rhs;
                    break;
                case OP_DIV:
                    res = lhs / rhs;
                    break;
                case OP_SIN:
                    res = sin(rhs);
                    break;
                case OP_COS:
                    res = cos(rhs);
                    break;
                case OP_EXP:
                    res = exp(log(lhs) * rhs);
                    break;
                case OP_LN:
                    res = log(rhs);
                    break;
            }
    }

    TreeNode *res_node = CREATE_NUM(res);
    LoggerLog(logger, CONV_TYPE_RESULT, res_node);
    free(res_node);

    return res;
}

TreeNode* Tangent(TreeNode *node, double x0, Logger *logger)
{
    LoggerLog(logger, CONV_TYPE_BEGIN_TANGENT, node);

    TreeNode *tangent = 
        ADD
        (
            MUL
            (
                CREATE_NUM(Evaluate(Differentiate(node, logger), x0, logger)),
                SUB(CREATE_VAR("x"), CREATE_NUM(x0))
            ),
            CREATE_NUM(Evaluate(node, x0, logger))
        );

    Simplify(tangent, logger);

    LoggerLog(logger, CONV_TYPE_RESULT_TANGENT, tangent);

    return tangent;
}

#undef CURR


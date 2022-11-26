#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "diffr.h"
#include "tree.h"
#include "iostr.h"
#include "stack.h"
#include "colors.h"
#include "parser.h"
#include "dsl.h"
#include "log.cpp"

const Option EXEC_OPTIONS[] = 
    {
        {"--help",     "-h",  HELP_OPTION,     "show help"}
    };
 
const size_t N_EXEC_OPTIONS = sizeof(EXEC_OPTIONS) / sizeof(Option);

void DiffrCtor(Diffr *diffr)
{
    ASSERT(diffr != NULL);

    diffr->root     = NULL;
    diffr->filename = NULL;
}

void DiffrDtor(Diffr *diffr)
{
    ASSERT(diffr != NULL);

    TreeDtor(diffr->root);
    free(diffr->filename);
}

void DiffrRun(Diffr *diffr)
{
    TreeNode *df_node = Differentiate(diffr->root, diffr);

    TreeDtor(diffr->root);
    diffr->root = df_node;

    Simplify(diffr->root);
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

    TextInfoDtor(&text);
}

#define DFR diffr

TreeNode* Differentiate(TreeNode *node, Diffr *diffr)
{
    ASSERT(node != NULL);

    switch (node->type)
    {
        case NODE_TYPE_NUM: 
            {
                LoggerLog(&diffr->logger, CONV_TYPE_CONST, CURR);
                return CREATE_NUM(0);
            }
        case NODE_TYPE_VAR:
            {
                LoggerLog(&diffr->logger, CONV_TYPE_SOLO_VAR, CURR);
                return CREATE_NUM(1);
            }
        case NODE_TYPE_OP:
            switch (node->value.op)
            {
                case OP_ADD: 
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_ADD, CURR);
                        return ADD(D_L, D_R);
                    }
                case OP_SUB:
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_ADD, CURR);
                        return SUB(D_L, D_R);
                    }

                case OP_MUL:
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_MUL, CURR);
                        return ADD(MUL(D_L, CP_R), MUL(CP_L, D_R));
                    }
                case OP_DIV:
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_DIV, CURR);
                        return DIV(SUB(MUL(D_L, CP_R), MUL(CP_L, D_R)), MUL(CP_R, CP_R));
                    }

                case OP_SIN:
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_SIN, CURR);
                        return MUL(COS(CP_R), D_R);
                    }
                case OP_COS:
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_COS, CURR);
                        return MUL(CREATE_NUM(-1), MUL(SIN(CP_R), D_R));
                    }

                case OP_EXP:
                    if (IS_NUM(LEFT) && IS_NUM(RIGHT))
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_EXP_CONST_CONST, CURR);
                        return CREATE_NUM(0);
                    }
                    else if (IS_NUM(LEFT) && IS_FUNC(RIGHT))
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_EXP_CONST_FUNC, CURR);
                        return MUL(EXP(CP_L, CP_R), MUL(LN(CP_L), D_R));
                    }
                    else if (IS_FUNC(LEFT) && IS_NUM(RIGHT))
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_EXP_FUNC_CONST, CURR);
                        return MUL(CP_R, MUL(EXP(CP_L, CREATE_NUM(GET_NUM(RIGHT) - 1)), D_L));
                    }
                    else
                        {
                            LoggerLog(&diffr->logger, CONV_TYPE_EXP_FUNC_FUNC, CURR);
                            TreeNode *fict_node = EXP(CREATE_NUM(exp(1)), MUL(LN(CP_L), CP_R));  // TODO: e const
                            TreeNode *res = Differentiate(fict_node, diffr);
                            TreeDtor(fict_node);

                            return res;
                        }
                
                case OP_LN:
                    {
                        LoggerLog(&diffr->logger, CONV_TYPE_LN, CURR);
                        return MUL(DIV(CREATE_NUM(1), CP_R), D_R);
                    }
            }
    }
}

#undef DFR

void Simplify(TreeNode *node)
{
    if (LEFT)
        Simplify (LEFT);

    if (RIGHT)
        Simplify (RIGHT);

    RotateCommutative (CURR);
    SimplifyConst     (CURR);
    SimplifyNeutral   (CURR);
}

void SimplifyConst(TreeNode *node)
{
    if (IS_VAR(node) || IS_NUM(node))
        return;
    
    switch (node->value.op)
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
}

void SimplifyNeutral(TreeNode *node)
{
    if (IS_NUM(node) || IS_VAR(node))
        return;

    switch (node->value.op)
    {
        case OP_ADD:
            if (IS_ZERO(LEFT))
            {
                free(LEFT);

                TreeNode *last_right =  RIGHT;
                     *node       = *RIGHT;

                free(last_right);
            }
            break;
        case OP_MUL:
            if (IS_ZERO(LEFT))
            {
                free(LEFT);
                TreeDtor(RIGHT);

                NUM_CTOR(node, 0);
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

#undef CURR

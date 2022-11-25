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

void DiffrInput(Diffr *diffr, const char *filename, int32_t *err)
{
    TextInfo text = {};
    TextInfoCtor(&text);

    InputText(&text, filename, err);
    InitTextSep(&text);

    diffr->root = NodeNew();
    GetGeneral(text.base, diffr->root);

    diffr->filename = strdup(filename);

    TextInfoDtor(&text);
}

void DiffrDump(Diffr *diffr)
{
    ASSERT(diffr != NULL);

    static int32_t cnt = 0;

    char filename_txt[256] = "",
         filename_svg[256] = "";

    sprintf(filename_txt, "dump/difft_%s_%d.txt", diffr->filename, cnt);
    sprintf(filename_svg, "dump/diffr_%s_%d.svg", diffr->filename, cnt++);

    int32_t fd = creat(filename_txt, S_IRWXU);
    ASSERT(fd != -1);

    dprintf(fd, "digraph G {\n");
    DiffrDumpToFileDfs(diffr->root, fd, 0);
    dprintf(fd, "}\n");

    close(fd);

    char cmd[256] = "";
    sprintf(cmd, "dot %s -o %s -Tsvg", filename_txt, filename_svg);
    system(cmd);
}

void DiffrDumpToFileDfs(Node *node, int32_t fd, int64_t idx)
{
    const char *type_str   = "";
          char  value[256] = "";

    switch (node->type)
    {
        case TYPE_OP:
            type_str = "OP";
            sprintf(value, "%s", GetOperatorString(node->value.op));
            break;
        case TYPE_NUM:
            type_str = "NUM";
            sprintf(value, "%lf", node->value.dbl);
            break;
        case TYPE_VAR:
            type_str = "VAR";
            sprintf(value, "%s", node->value.var);
            break;
    }

    dprintf(fd, "node%ld[shape=record, label=\" { %s | %s } \"];\n",
                idx, type_str, value);

    if (node->left != NULL)
    {
        dprintf(fd, "node%ld->node%ld;", idx, 2 * idx + 1);
        DiffrDumpToFileDfs(node->left,  fd, 2 * idx + 1);
    }

    if (node->right != NULL)
    {
        dprintf(fd, "node%ld->node%ld;", idx, 2 * idx + 2);
        DiffrDumpToFileDfs(node->right, fd, 2 * idx + 2);
    }
}

const char *GetOperatorString(int32_t op_code)
{
    switch (op_code)
    {
        case OP_ADD:
            return "{ ADD | +   }";
        case OP_SUB:
            return "{ SUB | -   }";
        case OP_MUL:
            return "{ MUL | *   }";
        case OP_DIV:
            return "{ DIV | /   }";
        case OP_SIN:
            return "{ SIN | sin }";
        case OP_COS:
            return "{ COS | cos }";
        case OP_EXP:
            return "{ EXP | ^   }";
        case OP_LN:
            return "{ LN  | ln  }";
    }

    return "(null)";
}

#define CURR node

Node* Differentiate(Node *node)
{
    ASSERT(node != NULL);

    switch (node->type)
    {
        case TYPE_NUM: return CREATE_NUM(0);
        case TYPE_VAR: return CREATE_NUM(1);
        case TYPE_OP:
            switch (node->value.op)
            {
                case OP_ADD: return ADD(D_L, D_R);
                case OP_SUB: return SUB(D_L, D_R);

                case OP_MUL: return ADD(MUL(D_L, CP_R), MUL(CP_L, D_R));
                case OP_DIV: return DIV(SUB(MUL(D_L, CP_R), MUL(CP_L, D_R)), MUL(CP_R, CP_R));

                case OP_SIN: return MUL(COS(CP_R), D_R);
                case OP_COS: return MUL(CREATE_NUM(-1), MUL(SIN(CP_R), D_R));

                case OP_EXP:
                    if (IS_NUM(LEFT) && IS_NUM(RIGHT))
                        return CREATE_NUM(0);
                    else if (IS_NUM(LEFT) && IS_FUNC(RIGHT))
                        return MUL(EXP(CP_L, CP_R), MUL(LN(CP_L), D_R));
                    else if (IS_FUNC(LEFT) && IS_NUM(RIGHT))
                        return MUL(CP_R, MUL(EXP(CP_L, CREATE_NUM(GET_NUM(RIGHT) - 1)), D_L));
                    else
                        {
                            Node *fict_node = EXP(CREATE_NUM(exp(1)), MUL(LN(CP_L), CP_R));  // TODO: e const
                            Node *res = Differentiate(fict_node);
                            TreeDtor(fict_node);

                            return res;
                        }
                
                case OP_LN:
                    return MUL(DIV(CREATE_NUM(1), CP_R), D_R);
            }
    }
}

#undef CURR

Node *CreateNode(int32_t type, NodeValue val, Node *left, Node *right)
{
    Node *node = NodeNew();

    NodeCtor(node, type, val, left, right);

    return node;
}

const char *GetGeneral(const char *str, Node *value)
{
    str = GetExpression(str, value);

    assert(*str == '\0');
    ++str;

    return str;
}

const char *GetExpression(const char *str, Node *value)
{
    Node *top_node = NodeNew();
    str = GetProduct(str, top_node);

    while (*str == '+' || *str == '-')
    {
        char op = *str;
        ++str;

        Node *buf_val = NodeNew();
        str = GetProduct(str, buf_val);

        switch (op)
        {
            case '+':
                top_node = ADD(top_node, buf_val);
                break;
            case '-':
                top_node = SUB(top_node, buf_val);
                break;
            default:
                assert(0 && "Syntax error\n");
        }
    }

    *value = *top_node;
    free(top_node);

    return str;
}

const char *GetProduct(const char *str, Node *value)
{
    Node *top_node = NodeNew();
    str = GetPower(str, top_node);

    while (*str == '*' || *str == '/')
    {
        char op = *str;
        ++str;

        Node *buf_val = NodeNew();
        str = GetPower(str, buf_val);

        switch (op)
        {
            case '*':
                top_node = MUL(top_node, buf_val);
                break;
            case '/':
                top_node = DIV(top_node, buf_val);
                break;
            default:
                assert(0 && "Syntax error\n");
        }
    }

    *value = *top_node;
    free(top_node);

    return str;
}

const char *GetPower(const char *str, Node *value)
{
    Node *top_node = NodeNew();
    str = GetPrimary(str, top_node);

    while (*str == '^')
    {
        Node *buf_val = NodeNew();

        ++str;
        str = GetPrimary(str, buf_val);

        top_node = EXP(top_node, buf_val);
    }

    *value = *top_node;
    free(top_node);

    return str;
}

const char *GetPrimary(const char *str, Node *value)
{
    if (*str == '(')
    {
        ++str;
        str = GetExpression(str, value);

        assert(*str == ')');
        ++str;
    }
    else if (isdigit(*str))
    {
        str = GetNumber(str, value);
    }
    else if (*str == 'x')
    {
        str = GetVariable(str, value);
    }
    else
    {
        str = GetFunction(str, value);
    }

    return str;
}

const char *GetNumber(const char *str, Node *value)
{
    int32_t res = 0;
    const char *str_old = str;

    while ('0' <= *str && *str <= '9')
    {
        res = res * 10 + *str - '0';
        ++str;
    }

    assert(str != str_old);

    NUM_CTOR(value, res);

    return str;
}

const char *GetVariable(const char *str, Node *value)
{
    const char *str_old = str;

    while (isalpha(*str))
        ++str;

    assert(str != str_old);

    *value = 
        {
            .type  = TYPE_VAR,
            .value = {.var = strndup(str_old, str - str_old)},
            .left  = NULL,
            .right = NULL
        };

    return str;
}

const char *GetFunction(const char *str, Node *value)
{
    int32_t op = 0;

    if (strncmp(str, "sin", 3) == 0)
    {
        op = OP_SIN;
        str += 3;
    }
    else if (strncmp(str, "cos", 3) == 0)
    {
        op = OP_COS;
        str += 3;
    }
    else if (strncmp(str, "ln", 2) == 0)
    {
        op = OP_LN;
        str += 2;
    }
    else
        assert(0 && "Wrong function");

    assert(*str == '(');
    ++str;

    Node *arg = NodeNew();
    str = GetExpression(str, arg);

    assert(*str == ')');
    ++str;

    OP_CTOR(value, op, NULL, arg);

    return str;
}

#define CURR node

void Simplify(Node *node)
{
    if (LEFT)
        Simplify (LEFT);

    if (RIGHT)
        Simplify (RIGHT);

    RotateCommutative (CURR);
    SimplifyConst     (CURR);
    SimplifyNeutral   (CURR);
}

void SimplifyConst(Node *node)
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
                    GET_TYPE(CURR) = TYPE_NUM;
                    
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
                    Node *last_right = RIGHT;
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
                    GET_TYPE(CURR) = TYPE_NUM;

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
                    Node *last_right = RIGHT;
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
                GET_TYPE(CURR) = TYPE_NUM;

                free(LEFT);
                LEFT = NULL;

                free(RIGHT);
                RIGHT = NULL;
            }
            else if (IS_NUM(LEFT)              &&
                     IS_OP_CODE(RIGHT, OP_MUL) &&
                     IS_NUM(RIGHT->left))
            {
                Node *last_right = RIGHT;
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
                GET_TYPE(CURR) = TYPE_NUM;

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
                GET_TYPE(CURR) = TYPE_NUM;

                free(RIGHT);
                RIGHT = NULL;
            }
            break;
        case OP_COS:
            if (IS_NUM(RIGHT))
            {
                GET_NUM(CURR) = cos(GET_NUM(RIGHT));
                GET_TYPE(CURR) = TYPE_NUM;

                free(RIGHT);
                RIGHT = NULL;
            }
            break;
        case OP_EXP:
            if (IS_NUM(LEFT) && IS_NUM(RIGHT))
            {
                GET_NUM(CURR) = pow(GET_NUM(LEFT), GET_NUM(RIGHT));
                GET_TYPE(CURR) = TYPE_NUM;

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
                GET_TYPE(CURR) = TYPE_NUM;

                free(RIGHT);
                RIGHT = NULL;
            }
            break;
        default:
            assert(0 && "Wrong operation");
    }
}

void SimplifyNeutral(Node *node)
{
    if (IS_NUM(node) || IS_VAR(node))
        return;

    switch (node->value.op)
    {
        case OP_ADD:
            if (IS_ZERO(LEFT))
            {
                free(LEFT);

                Node *last_right =  RIGHT;
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

                Node *last_right =  RIGHT;
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

                Node *last_left =  LEFT;
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
            else if (IS_ONE(LEFT))
            {
                free(LEFT);
                TreeDtor(RIGHT);

                NUM_CTOR(CURR, 1);
            }
    }
}

void RotateCommutative(Node *node)
{
    if ((IS_OP_CODE(CURR, OP_ADD) || IS_OP_CODE(CURR, OP_MUL)) && IS_NUM(RIGHT))
    {
        Node *buf   = LEFT;
              LEFT  = RIGHT;
              RIGHT = buf;
    }
}

#undef CURR

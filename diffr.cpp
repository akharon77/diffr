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

    if (!NodeIsLeaf(node))
    {
        dprintf(fd, "node%ld->node%ld;", idx, 2 * idx + 1);
        DiffrDumpToFileDfs(node->left,  fd, 2 * idx + 1);

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

    node->type  = type;
    node->left  = left;
    node->right = right;
    node->value = val;

    // switch (type)
    // {
    //     case TYPE_OP:
    //         node->value.op = val;
    //         break;
    //     case TYPE_VAR:
    //         node->value.var = val;
    //         break;
    //     case TYPE_NUM:
    //         node->value.num = val;
    //         break;
    //     default:
    // }

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
    str = GetTerm(str, top_node);

    while (*str == '+' || *str == '-')
    {
        char op = *str;
        ++str;

        Node *buf_val = NodeNew();
        str = GetTerm(str, buf_val);

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

const char *GetTerm(const char *str, Node *value)
{
    Node *top_node = NodeNew();
    str = GetPrimary(str, top_node);

    while (*str == '*' || *str == '/')
    {
        char op = *str;
        ++str;

        Node *buf_val = NodeNew();
        str = GetPrimary(str, buf_val);

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
    else
    {
        str = GetVariable(str, value);
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

    *value = 
        {
            .type  = TYPE_NUM,
            .value = {.dbl = res},
            .left  = NULL,
            .right = NULL
        };

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

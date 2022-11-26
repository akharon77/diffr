#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "parser.h"
#include "tree.h"
#include "diffr.h"

const char *GetGeneral(const char *str, TreeNode *value)
{
    str = GetExpression(str, value);

    assert(*str == '\0');
    ++str;

    return str;
}

const char *GetExpression(const char *str, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();
    str = GetProduct(str, top_node);

    while (*str == '+' || *str == '-')
    {
        char op = *str;
        ++str;

        TreeNode *buf_val = TreeNodeNew();
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

const char *GetProduct(const char *str, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();
    str = GetPower(str, top_node);

    while (*str == '*' || *str == '/')
    {
        char op = *str;
        ++str;

        TreeNode *buf_val = TreeNodeNew();
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

const char *GetPower(const char *str, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();
    str = GetPrimary(str, top_node);

    while (*str == '^')
    {
        TreeNode *buf_val = TreeNodeNew();

        ++str;
        str = GetPrimary(str, buf_val);

        top_node = EXP(top_node, buf_val);
    }

    *value = *top_node;
    free(top_node);

    return str;
}

const char *GetPrimary(const char *str, TreeNode *value)
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

const char *GetNumber(const char *str, TreeNode *value)
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

const char *GetVariable(const char *str, TreeNode *value)
{
    const char *str_old = str;

    while (isalpha(*str))
        ++str;

    assert(str != str_old);

    *value = 
        {
            .type  = NODE_TYPE_VAR,
            .value = {.var = strndup(str_old, str - str_old)},
            .left  = NULL,
            .right = NULL
        };

    return str;
}

const char *GetFunction(const char *str, TreeNode *value)
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

    TreeNode *arg = TreeNodeNew();
    str = GetExpression(str, arg);

    assert(*str == ')');
    ++str;

    OP_CTOR(value, op, NULL, arg);

    return str;
}


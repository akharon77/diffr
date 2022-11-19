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

    diffr->root = NULL;
}

void DiffrInput(Diffr *diffr, const char *filename, int32_t *err)
{
    TextInfo text = {};
    TextInfoCtor(&text);

    InputText(&text, filename, err);

    int32_t end = 0;
    diffr->root = DiffrParse(0, &end, &text);

    TextInfoDtor(&text);
}

Node *DiffrParse(int32_t pos, int32_t *end_pos, TextInfo *text)
{
    Node *node = NodeNew();

    if (text->base[pos + 1] == '(')
    {
        int32_t i = 0;
        NodeAddChild(node, DiffrParse(pos + 1, &i, text));
        
        int32_t op_code = 0;

        switch (text->base[++i])
        {
            case '+':
                op_code = OP_ADD;
                break;
            case '-':
                op_code = OP_SUB;
                break;
            case '*':
                op_code = OP_MUL;
                break;
            case '/':
                op_code = OP_DIV;
                break;
            case 's':
                op_code = OP_SIN;
                break;
            case 'c':
                op_code = OP_COS;
                break;
            case '^':
                op_code = OP_EXP;
                break;
            default:
                ASSERT(0 && RED "Wrong operator!" NORMAL);
        }

        node->type     = TYPE_OP;
        node->value.op = op_code;

        NodeAddChild(node, DiffrParse(i + 1, end_pos, text));

        ++*end_pos;
    }
    else if (isalpha(text->base[pos + 1]))
    {
        node->type      = TYPE_VAR;
        node->value.var = text->base[pos + 1];

        *end_pos = pos + 2;
    }
    else
    {
        node->type = TYPE_NUM;
        
        double  num = 0;
        int32_t offset = 0;

        sscanf(text->base + pos + 1, "%lf)%n", &num, &offset);

        node->value.dbl = num;
            
        *end_pos = pos + offset;
    }

    return node;
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
            sprintf(value, "%c", node->value.var);
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
            return "{ ADD | + }";
        case OP_SUB:
            return "{ SUB | - }";
        case OP_MUL:
            return "{ MUL | * }";
        case OP_DIV:
            return "{ DIV | / }";
        case OP_SIN:
            return "{ SIN }";
        case OP_COS:
            return "{ COS }";
        case OP_EXP:
            return "{ EXP | ^ }";
    }

    return "(null)";
}

#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "diffr_debug.h"
#include "dsl.h"

void TreeDump(TreeNode *node, const char *filename)
{
    ASSERT(node != NULL);

    static int32_t cnt = 0;

    char filename_txt[512] = "",
         filename_svg[512] = "";

    sprintf(filename_txt, "dump/%s_%d.txt", filename, cnt);
    sprintf(filename_svg, "dump/%s_%d.svg", filename, cnt++);

    int32_t fd = creat(filename_txt, S_IRWXU);
    ASSERT(fd != -1);

    dprintf(fd, "digraph G {\n");
    TreeDumpToFile(node, fd, 0);
    dprintf(fd, "}\n");

    close(fd);

    char cmd[512] = "";
    sprintf(cmd, "dot %s -o %s -Tsvg", filename_txt, filename_svg);
    system(cmd);
}

#define CURR node

void TreeDumpToFile(TreeNode *node, int32_t fd, int64_t idx)
{
    const char *type_str   = "";
          char  value[256] = "";

    switch (GET_TYPE(CURR))
    {
        case NODE_TYPE_OP:
            type_str = "OP";
            sprintf(value, "%s", GetOperatorString(GET_OP(CURR)));
            break;
        case NODE_TYPE_NUM:
            type_str = "NUM";
            sprintf(value, "%lf", GET_NUM(CURR));
            break;
        case NODE_TYPE_VAR:
            type_str = "VAR";
            sprintf(value, "%s", GET_VAR(CURR));
            break;
    }

    dprintf(fd, "node%ld[shape=record, label=\" { %s | %s | %d } \"];\n",
                idx, type_str, value, node->size);

    if (LEFT)
    {
        dprintf(fd, "node%ld->node%ld;", idx, 2 * idx + 1);
        TreeDumpToFile(LEFT,  fd, 2 * idx + 1);
    }

    if (RIGHT)
    {
        dprintf(fd, "node%ld->node%ld;", idx, 2 * idx + 2);
        TreeDumpToFile(RIGHT, fd, 2 * idx + 2);
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

const char *GetOperatorString(int32_t op_code)
{
    switch (op_code)
    {
        case OP_ADD:
            return "{ADD | +}";
        case OP_SUB:
            return "{SUB | -}";
        case OP_MUL:
            return "{MUL | *}";
        case OP_DIV:
            return "{DIV | /}";
        case OP_SIN:
            return "{SIN | sin}";
        case OP_COS:
            return "{COS | cos}";
        case OP_EXP:
            return "{EXP | ^}";
        case OP_LN:
            return "{LN | ln}";
        default:
            return "(null)";
    }
}

int32_t GetOperatorPriority(int32_t op_code)
{
    switch (op_code)
    {
        case OP_ADD:
        case OP_SUB:
            return 0;
        case OP_MUL:
        case OP_DIV:
            return 1;
        case OP_SIN:
        case OP_COS:
        case OP_LN:
            return 2;
        case OP_EXP:
            return 3;
    }
}


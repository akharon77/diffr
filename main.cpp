#include <stdio.h>

#include "diffr.h"
#include "colors.h"

int main(int argc, const char *argv[])
{
    // Node *root = NodeNew();
    // GetExpression("x*0+2*1", root);
    // DiffrDumpToFileDfs(root, 1, 0);
    // printf("\n");
    // Simplify(root);
    // DiffrDumpToFileDfs(root, 1, 0);

    // return 0;

    int err = 0;

    int run_mode = HELP_OPTION;
    const char *filename = "formula.txt";

    int options[N_EXEC_OPTIONS] = {};

    bool ok = GetOptions(argc, argv, options, EXEC_OPTIONS, N_EXEC_OPTIONS);
    if (!ok)
    {
        printf(RED "Wrong arguments\n" NORMAL);
        return 1;
    }

    Diffr diffr = {};

    DiffrInput(&diffr, "test.txt", &err);
    DiffrDump(&diffr);

    DiffrRun(&diffr);
    DiffrDump(&diffr);
    
    DiffrDtor(&diffr);

    return 0;
}

#include <stdio.h>

#include "diffr.h"
#include "colors.h"

int main(int argc, const char *argv[])
{
    const char *str = argv[1];
    Node *ans = NodeNew();

    GetGeneral(str, ans);

    DiffrDumpToFileDfs(ans, 1, 0);

    TreeDtor(ans);
    return 0;

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

    diffr.filename = "test.txt";
    DiffrInput(&diffr, "test.txt", &err);
    DiffrDump(&diffr);

    Diffr df = {};
    df.root = Differentiate(diffr.root);
    df.filename = "df.txt";
    DiffrDump(&df);
    
    return 0;
}

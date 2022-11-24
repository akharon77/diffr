#include <stdio.h>

#include "diffr.h"
#include "colors.h"

int main(int argc, const char *argv[])
{
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
    df.filename = "df.txt";
    df.root = Differentiate(diffr.root);
    DiffrDump(&df);
    Simplify(df.root);
    DiffrDump(&df);
    
    return 0;
}

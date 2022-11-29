#include <stdio.h>

#include "diffr.h"
#include "parser.h"
#include "colors.h"
#include "log.h"

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

    DiffrCtor(&diffr);
    DiffrInput(&diffr, "test.txt", &err);
    DiffrDump(&diffr);

    DiffrRun(&diffr);
    DiffrDump(&diffr);

    LoggerGenerateToFdLatexBook(&diffr.logger, 1);
    
    DiffrDtor(&diffr);

    return 0;
}

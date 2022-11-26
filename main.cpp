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

    DiffrInput(&diffr, "test.txt", &err);
    DiffrDump(&diffr);

    DiffrRun(&diffr);
    DiffrDump(&diffr);

    for (int32_t i = 0; i < diffr.logger.convs.size; ++i)
    {
        char str[1024] = "";
        LoggerPrintToStrLatex(&diffr.logger, str, i);
        printf("%s \n \\\\ \n\n", str);
    }
    
    DiffrDtor(&diffr);

    return 0;
}

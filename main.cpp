#include <stdio.h>
#include <string.h>

#include "diffr.h"
#include "parser.h"
#include "colors.h"
#include "log.h"

int32_t main(int32_t argc, const char *argv[])
{
    int32_t err = 0;

    int32_t run_mode = HELP_OPTION;
    int32_t df_n  = 3;
    double  df_x0 = 0;

    const char *filename = "formula.txt";

    int options[N_EXEC_OPTIONS] = {};

    bool ok = GetOptions(argc, argv, options, EXEC_OPTIONS, N_EXEC_OPTIONS);
    if (!ok)
    {
        printf(RED "Wrong arguments\n" NORMAL);
        return 1;
    }

    if (options[DF_OPTION])
        run_mode = DF_OPTION;

    if (run_mode == HELP_OPTION)
    {
        for (int32_t i = 0; i < N_EXEC_OPTIONS; ++i)
            printf("%10s %5s %s\n",
                    EXEC_OPTIONS[i].strFormLong,
                    EXEC_OPTIONS[i].strFormShort,
                    EXEC_OPTIONS[i].description);
        return 0;
    }

    if (options[FILENAME_OPTION])
        filename = argv[options[FILENAME_OPTION] + 1];
    if (options[DF_N_OPTION])
        df_n = atol(argv[options[DF_N_OPTION] + 1]);
    if (options[DF_X0_OPTION])
        df_x0 = atof(argv[options[DF_X0_OPTION] + 1]);

    Diffr diffr = {};

    DiffrCtor(&diffr, df_x0, df_n);
    DiffrInput(&diffr, filename, &err);
    DiffrDump(&diffr);

    DiffrRun(&diffr);
    DiffrDump(&diffr);

    int32_t fd = creat("output.tex", S_IRWXU);
    DiffrGenerateFdLatexBook(&diffr, fd);
    close(fd);
    
    DiffrDtor(&diffr);

    return 0;
}

#include <stdio.h>

#include "akinator.h"
#include "colors.h"

int main(int argc, const char *argv[])
{
    int err = 0;

    int run_mode = HELP_OPTION;
    const char *db_filename = "akinator_db.txt",
               *obj1        = "null1",
               *obj2        = "null2";

    int options[N_EXEC_OPTIONS] = {};

    bool ok = GetOptions(argc, argv, options, EXEC_OPTIONS, N_EXEC_OPTIONS);
    if (!ok)
    {
        printf(RED "Wrong arguments\n" NORMAL);
        return 1;
    }
    
    return 0;
}

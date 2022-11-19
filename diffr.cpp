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

    diffr.root = NULL;
}

void DiffrInput(Diffr *diffr, const char *filename, int32_t *err)
{
    TextInfo text = {};
    TextCtor(&text);

    InputText(&text, filename, err);

    DiffrParse(diffr->root, 0, text->base);

    TextInfoDtor(&text);
}

void DiffrParse(Node *node, int32_t pos, char *base)
{
    
}

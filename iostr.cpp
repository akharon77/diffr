#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include "iostr.h"
#include "colors.h"

const int MAX_LINE_LEN = 128;

ErrorTag ERROR_TAGS[] =
    {
        {"",                                            NO_ERROR                },
        {"Error in reading file",                       FILE_READING_ERROR      },
        {"Error in reading file stats",                 FILE_STATS_READING_ERROR},
        {"Error in opening file",                       FILE_OPEN_ERROR         },
        {"Wrong filename",                              FILE_NAME_ERROR         },
        {"Error in allocation memory for TextInfo",     TEXT_MALLOC_ERROR       },
        {"Error in allocation memory for lines (Line)", LINES_MALLOC_ERROR      },
        {"Null pointer to the text",                    TEXT_MARKOUT_NULL_ERROR }
    };

const char* ERRORS[N_ERRORS] = {};

bool GetOptions(const int argc, const char *argv[], int optionsInd[], const Option exec_options[], int n_exec_options)
{
    ASSERT(argv != NULL);

    for (int i = 1; i < argc; ++i)
        for (size_t j = 0; j < n_exec_options; ++j)
            if (strcmp(exec_options[j].strFormLong, argv[i])  == 0 ||
                strcmp(exec_options[j].strFormShort, argv[i]) == 0)
            {
                int id = exec_options[j].optionId;
                if (optionsInd[id])
                    return false;

                optionsInd[id] = i;
            }

    return true;
}

void TextInfoCtor(TextInfo *text)
{
    text->nlines = text->size = 0;
    text->lines = NULL;
    text->base  = NULL;
}

void InputText(TextInfo *text, const char *filename, int *err)
{
    *err = NO_ERROR;

    RET_ERR(filename == NULL, err, FILE_NAME_ERROR);

    int fd = open(filename, O_RDONLY, 0);
    RET_ERR(fd == -1, err, FILE_OPEN_ERROR);

    struct stat fileStatBuf = {};
    RET_ERR(fstat(fd, &fileStatBuf) != 0, err, FILE_STATS_READING_ERROR);

    size_t  fileSize = (size_t) fileStatBuf.st_size;
    char   *fileCont = (char*)  calloc(fileSize, sizeof(char));

    RET_ERR(fileCont == NULL, err, FILE_CONTENT_MALLOC_ERROR);

    ssize_t n_read = read(fd, fileCont, fileSize);
    RET_ERR(n_read < (ssize_t) fileSize, err, FILE_READING_ERROR);

    text->size = fileSize;
    text->base = fileCont;

    close(fd);
}

void InitTextSep(TextInfo *text)
{
    ASSERT(text != NULL);

    size_t res   = 0;
    bool   empty = true;

    for (size_t i = 0; i < text->size; ++i)
    {
        if (!isspace(text->base[i]))
            empty = false;

        if (text->base[i] == '\n')
        {
            if (!empty)
            {
                ++res;
                text->base[i] = '\0';
            }
            empty = true;
        }
    }

    text->nlines = res;
}

void MarkOutText(TextInfo *text, int *err)
{
    ASSERT(text != NULL);
    ASSERT(err  != NULL);
    
    *err = NO_ERROR;

    RET_ERR(text == NULL, err, TEXT_MARKOUT_NULL_ERROR);

    InitTextSep(text);
    text->lines = (Line*) calloc(text->nlines, sizeof(Line));

    bool isLine = false;
    int lastNotSpace = 0;
    size_t currnLines = 0;
    for (int i = 0; i < (int) text->size; ++i)
    {
        if (!(isspace(text->base[i]) || text->base[i] == '\0'))
            lastNotSpace = i;

        if (text->base[i] == '\0')
        {
            text->base[lastNotSpace + 1] = '\0';
            text->lines[currnLines - 1].len = text->base + lastNotSpace - text->lines[currnLines - 1].ptr + 1;
            isLine = false;
        }
        else if (!isLine && !(isspace(text->base[i]) || text->base[i] == '\0'))
        {
            isLine = true;
            text->lines[currnLines].ptr = text->base + i;
            text->lines[currnLines].pos = currnLines;
            ++currnLines;
        }
    }
}

void TextInfoDtor(TextInfo *text)
{
    free(text->base);
    free(text->lines);

    text->base  = NULL;
    text->lines = NULL;
}

void InitErrorTags()
{
    for (int i = 0; i < N_ERRORS; ++i)
        ERRORS[ERROR_TAGS[i].errorId] = ERROR_TAGS[i].description;
}


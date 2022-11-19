#ifndef IOSTR_H
#define IOSTR_H

#include <stdio.h>

#define ASSERT(expr)                                        \
do {                                                        \
    if (!(expr))                                            \
        fprintf(stderr,                                     \
                "Assertion failed. Expression: " #expr "\n" \
                "File: %s, line: %d\n"                      \
                "Function: %s\n",                           \
                __FILE__, __LINE__,                         \
                __PRETTY_FUNCTION__);                       \
} while(0)

#define LOG_ERROR(errorID)              \
do {                                    \
    if ((errorID) != NO_ERROR)          \
        fprintf(stderr,                 \
                "%s\n"                  \
                "File: %s, line: %d\n", \
                ERRORS[(errorID)],      \
                __FILE__, __LINE__);    \
} while (0)

#define RET_ERR(expr, errPtr, errId) \
do {                                 \
    if (expr)                        \
    {                                \
        *(errPtr) = (errId);         \
        return;                      \
    }                                \
} while (0)

enum ERRORS
{
    NO_ERROR,
    FILE_READING_ERROR,
    FILE_OPEN_ERROR,
    FILE_STATS_READING_ERROR,
    FILE_NAME_ERROR,
    FILE_CONTENT_MALLOC_ERROR,
    TEXT_MALLOC_ERROR,
    LINES_MALLOC_ERROR,
    TEXT_MARKOUT_NULL_ERROR,
    N_ERRORS
};

struct ErrorTag
{
    const char *description;
    int         errorId;
};

struct Option
{
    const char *strFormLong,
               *strFormShort;
    int         optionId;
    const char *description;
};

struct Line
{
    const char *ptr;
    ssize_t     len;
    int         pos;
};

struct TextInfo
{
    size_t nlines,
           size;
    Line  *lines;
    char  *base;
};

extern       ErrorTag ERROR_TAGS[];
extern const char*    ERRORS[];
extern const int      MAX_LINE_LEN;

void TextInfoCtor  (TextInfo *text);
void TextInfoDtor  (TextInfo *text);
void InputText     (TextInfo *text, const char *filename, int *err);
void InitTextSep   (TextInfo *text);
void MarkOutText   (TextInfo *text, int *err);
bool GetOptions    (const int argc, const char *argv[], int optionsInd[], const Option exec_options[], int n_exec_options);
void InitErrorTags ();

#endif  //IOSTR_H


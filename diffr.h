#ifndef AKINATOR_H
#define AKINATOR_H

#include "iostr.h"
#include "tree.h"
#include "stack.h"

enum OPTIONS
{
    DB_FILE_OPTION,
    HELP_OPTION,
    PREDICT_OPTION,
    COMPARE_OPTION,
    DESCRIBE_OPTION,
    N_OPTIONS
};

enum AKINATOR_STATES
{
    VERTEX_IN,
    VERTEX_LEFT,
    VERTEX_RIGHT
};

extern const Option       EXEC_OPTIONS[];
extern const size_t       N_EXEC_OPTIONS;

const int32_t MAX_STR_OBJ_LEN = 128;

struct Akinator
{
    Node *root;

    char *db_filename;
};

void AkinatorCtor          (Akinator *aktr, const char *db_filename, int *err);
void AkinatorDtor          (Akinator *aktr);

void AkinatorPredict       (Akinator *aktr);
void AkinatorCompare       (Akinator *aktr, const char *obj1, const char *obj2);
void AkinatorDescribe      (Akinator *aktr, const char *obj);

void AkinatorPrintByPath   (Node *node, Stack *stk);
bool AkinatorFindObj       (Node *node, const char *str, Stack *stk);

void AkinatorParseText     (Akinator *aktr, TextInfo *text);
void AkinatorSaveDbToFile  (Akinator *aktr);
void AkinatorSaveDfs       (Node *node, int32_t depth, int32_t fd);

void AkinatorDumpToFile    (Akinator *aktr, const char *filename);
void AkinatorDumpToFileDfs (Node *node, int32_t fd, int64_t idx);

bool GetAnsYesNo();

#endif  // AKINATOR_H

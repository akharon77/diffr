#ifndef DIFFR_H
#define DIFFR_H

#include "iostr.h"
#include "tree.h"
#include "stack.h"

#define CreateNum(val) CreateNode(TYPE_NUM, val, NULL, NULL)
#define dL             DiffrDifferentiate(LEFT)
#define dR             DiffrDifferentiate(RIGHT)
#define cL             TreeCopy(LEFT)
#define cR             TreeCopy(RIGHT)

enum OPTIONS
{
    DB_FILE_OPTION,
    HELP_OPTION,
    PREDICT_OPTION,
    COMPARE_OPTION,
    DESCRIBE_OPTION,
    N_OPTIONS
};

enum NODE_TYPES
{
    TYPE_OP,
    TYPE_NUM,
    TYPE_VAR
};

enum OP_CODES
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_SIN,
    OP_COS,
    OP_EXP
};

struct Diffr
{
    char *filename;

    Node *root;
};

extern const Option       EXEC_OPTIONS[];
extern const size_t       N_EXEC_OPTIONS;

const int32_t MAX_STR_OBJ_LEN = 128;

void        DiffrCtor          (Diffr *diffr);

void        DiffrInput         (Diffr *diffr, const char *filename, int32_t *err);
Node       *DiffrParse         (int32_t pos, int32_t *end_pos, TextInfo *text);

void        DiffrDump          (Diffr *diffr);
void        DiffrDumpToFileDfs (Node *node, int32_t fd, int64_t idx);

const char *GetOperatorString  (int32_t op_code);
#endif  // DIFFR_H

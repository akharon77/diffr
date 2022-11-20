#ifndef DIFFR_H
#define DIFFR_H

#include "iostr.h"
#include "tree.h"
#include "stack.h"

#define CREATE_NUM(val) CreateNode (TYPE_NUM, val, NULL, NULL)

#define LEFT           (CURR)->left
#define RIGHT          (CURR)->right

#define DL             DiffrDifferentiate (LEFT)
#define DR             DiffrDifferentiate (RIGHT)

#define CL             TreeCopy (LEFT)
#define CR             TreeCopy (RIGHT)

#define ADD(lhs, rhs)  CreateNode (TYPE_OP, OP_ADD, lhs,          rhs)
#define SUB(lhs, rhs)  CreateNode (TYPE_OP, OP_SUB, lhs,          rhs)

#define MUL(lhs, rhs)  CreateNode (TYPE_OP, OP_MUL, lhs,          rhs)
#define DIV(lhs, rhs)  CreateNode (TYPE_OP, OP_DIV, lhs,          rhs)

#define SIN(rhs)       CreateNode (TYPE_OP, OP_SIN, CreateNum(0), rhs)
#define COS(rhs)       CreateNode (TYPE_OP, OP_COS, CreateNum(0), rhs)

#define EXP(lhs, rhs)  CreateNode (TYPE_OP, OP_EXP, lhs,          rhs)
#define LN(rhs)        CreateNode (TYPE_OP, OP_LN,  CreateNum(0), rhs)

#define IS_OP(node, op_code) ((node)->type     == TYPE_OP && \
                              (node)->value.op == op_code)
#define IS_NUM(node) ((node)->type == TYPE_NUM)
#define IS_VAR(node) ((node)->type == TYPE_VAR)

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

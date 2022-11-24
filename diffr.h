#ifndef DIFFR_H
#define DIFFR_H

#include "iostr.h"
#include "tree.h"
#include "stack.h"

#define NUM_CTOR(node, val)   NodeCtor   (node,     TYPE_NUM, {.dbl = val}, NULL, NULL)
#define CREATE_NUM(val)       CreateNode (          TYPE_NUM, {.dbl = val}, NULL, NULL)

#define LEFT           (CURR)->left
#define RIGHT          (CURR)->right

#define D_L            Differentiate (LEFT)
#define D_R            Differentiate (RIGHT)

#define CP_L           TreeCopy (LEFT)
#define CP_R           TreeCopy (RIGHT)
#define CP_CR          TreeCopy (CURR)

#define ADD(lhs, rhs)  CreateNode (TYPE_OP, {.op = OP_ADD}, lhs,          rhs)
#define SUB(lhs, rhs)  CreateNode (TYPE_OP, {.op = OP_SUB}, lhs,          rhs)

#define MUL(lhs, rhs)  CreateNode (TYPE_OP, {.op = OP_MUL}, lhs,          rhs)
#define DIV(lhs, rhs)  CreateNode (TYPE_OP, {.op = OP_DIV}, lhs,          rhs)

#define SIN(rhs)       CreateNode (TYPE_OP, {.op = OP_SIN}, CREATE_NUM(0), rhs)
#define COS(rhs)       CreateNode (TYPE_OP, {.op = OP_COS}, CREATE_NUM(0), rhs)

#define EXP(lhs, rhs)  CreateNode (TYPE_OP, {.op = OP_EXP}, lhs,           rhs)
#define LN(rhs)        CreateNode (TYPE_OP, {.op = OP_LN},  CREATE_NUM(0), rhs)

#define IS_OP(node)               (GET_TYPE(node) == TYPE_OP)
#define IS_OP_CODE(node, op_code) (IS_OP(node) && GET_OP(node) == op_code)

#define IS_NUM(node)              (GET_TYPE(node) == TYPE_NUM)

#define EPS                        1e-6
#define IS_EQ(node, val)          (IS_NUM(node) && (val) - EPS < GET_NUM(node) && GET_NUM(node) < (val) + EPS)
#define IS_ZERO(node)             (IS_EQ(node, 0))
#define IS_ONE(node)              (IS_EQ(node, 1))

#define IS_VAR(node)              (GET_TYPE(node) == TYPE_VAR)

#define IS_FUNC(node)             (IS_OP(node) || IS_VAR(node))

#define GET_TYPE(node)            ((node)->type)
#define GET_NUM(node)             ((node)->value.dbl)
#define GET_OP(node)              ((node)->value.op)

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
    OP_EXP,
    OP_LN
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

Node       *Differentiate      (Node *node);

void        Simplify           (Node *node);
void        SimplifyConst      (Node *node);
void        SimplifyNeutral    (Node *node);
void        RotateCommutative  (Node *node);

Node       *CreateNode         (int32_t type, NodeValue val, Node *left, Node *right);

const char *GetOperatorString  (int32_t op_code);

const char *GetGeneral    (const char *str, Node *value);
const char *GetExpression (const char *str, Node *value);
const char *GetTerm       (const char *str, Node *value);
const char *GetPrimary    (const char *str, Node *value);
const char *GetNumber     (const char *str, Node *value);
const char *GetVariable   (const char *str, Node *value);

#endif  // DIFFR_H

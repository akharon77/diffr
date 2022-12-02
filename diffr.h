#ifndef DIFFR_H
#define DIFFR_H

#include "iostr.h"
#include "tree.h"
#include "stack.h"
#include "dsl.h"
#include "log.h"
#include "diffr_struct.h"

enum OPTIONS
{
    HELP_OPTION,
    FILENAME_OPTION,
    DF_OPTION,
    DF_N_OPTION,
    DF_X0_OPTION,
    N_OPTIONS
};

extern const Option       EXEC_OPTIONS[];
extern const size_t       N_EXEC_OPTIONS;

const int32_t MAX_STR_OBJ_LEN = 128;

void        DiffrCtor           (Diffr *diffr, double df_x0, int32_t df_n);
void        DiffrDtor           (Diffr *diffr);

void        DiffrRun            (Diffr *diffr);

void        DiffrInput          (Diffr *diffr, const char *filename, int32_t *err);
TreeNode   *DiffrParse          (int32_t pos, int32_t *end_pos, TextInfo *text);

void        DiffrDump           (Diffr *diffr);
void        DumpToFile          (TreeNode *node, int32_t fd, int64_t idx);

TreeNode   *Differentiate       (TreeNode *node, Logger *logger);
TreeNode   *TaylorSeries        (TreeNode *node, double x0, int32_t n, Logger *logger);

double      Evaluate            (TreeNode *node, double x, Logger *logger);

void        Simplify            (TreeNode *node, Logger *logger);
void        SimplifyConst       (TreeNode *node, Logger *logger);
void        SimplifyNeutral     (TreeNode *node, Logger *logger);

void        Rotate              (TreeNode *node);
void        RotateCommutative   (TreeNode *node);
void        RotateSizeMin       (TreeNode *node);

const char *GetOperatorString   (int32_t op_code);

#endif  // DIFFR_H

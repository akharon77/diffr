#ifndef LOG_H
#define LOG_H

#include "diffr_struct.h"
#include "log_struct.h"

void        LoggerCtor                  (Logger *logger, Diffr *diffr);
void        LoggerDtor                  (Logger *logger);
void        LoggerLog                   (Logger *logger, int32_t type, TreeNode *node);

void        LoggerReplace               (Logger *logger, TreeNode *node);

const char *GetRandomComment();
const char *GetConvDesc                 (int32_t type);
const char *GetGreekAlphabet            (int32_t id);
const char *GetOperatorStringPlot       (int32_t op_code);

void        DiffrGenerateFdLatexBook    (Diffr *diffr, int32_t fd);
void        LoggerPrintToFdLatex        (Logger *logger, int32_t fd, int32_t id);
void        PrintToFdLatex              (TreeNode *node, int32_t fd);
void        PrintToFdPlot               (TreeNode *node, int32_t fd);
void        PrintGraphToFile            (TreeNode *node, const char *filename);

#endif  // LOG_H


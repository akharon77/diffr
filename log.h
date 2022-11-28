#ifndef LOG_H
#define LOG_H

#include "diffr_struct.h"
#include "log_struct.h"

void        LoggerCtor            (Logger *logger, Diffr *diffr);
void        LoggerDtor            (Logger *logger);
void        LoggerLog             (Logger *logger, int32_t type, TreeNode *node);

void        LoggerReplace         (Logger *logger, TreeNode *node);

const char *GetConvDesc           (int32_t type);
const char *GetGreekAlphabet      (int32_t id);

void        LoggerPrintToStrLatex (Logger *log, char *str, int32_t id);
char       *PrintToStrLatex       (TreeNode *node, char *str);

#endif  // LOG_H

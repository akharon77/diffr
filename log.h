#ifndef LOG_H
#define LOG_H

#include "diffr_struct.h"
#include "log_struct.h"

void        LoggerCtor            (Logger *logger, Diffr *diffr);
void        LoggerDtor            (Logger *logger);
void        LoggerLog             (Logger *logger, int32_t type, TreeNode *node);

const char *GetConvDesc           (int32_t type);

void        LoggerPrintToStrLatex (Logger *log, char *str, int32_t id);
char       *PrintToStrLatex       (TreeNode *node, char *str);

#endif  // LOG_H

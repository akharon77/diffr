#ifndef LOG_H
#define LOG_H

#include "diffr_struct.h"
#include "log_struct.h"

void        LoggerCtor                  (Logger *logger, Diffr *diffr);
void        LoggerDtor                  (Logger *logger);
void        LoggerLog                   (Logger *logger, int32_t type, TreeNode *node);

void        LoggerReplace               (Logger *logger, TreeNode *node);

const char *GetConvDesc                 (int32_t type);
const char *GetGreekAlphabet            (int32_t id);

void        LoggerGenerateToFdLatexBook (Logger *logger, int32_t fd);
void        LoggerPrintToFdLatex        (Logger *log, char *str, int32_t id);
void        PrintToFdLatex              (TreeNode *node, int32_t fd);

#endif  // LOG_H

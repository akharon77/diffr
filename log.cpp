#include <stdlib.h>
#include <string.h>

#include "log.h"

void LoggerCtor(Logger *logger, Diffr *diffr)
{
    StackCtor(
    logger->filename = strdup(filename);
}

void LoggerDtor(Logger *logger)
{
    ListDtor(&logger->convs);
    free(logger->filename);
}

void LoggerLog(Logger *logger, int32_t type, TreeNode *node)
{
    ListPushBack(&logger->convs, 
                {
                    .type = type, 
                    .node = TreeCopy(node)
                });
}

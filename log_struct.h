#ifndef LOG_STRUCT_H
#define LOG_STRUCT_H

#include "tree_struct.h"
#include "stack_struct.h"

struct Logger
{
    Stack convs;
    
    char *filename;
};

#endif  // LOG_STRUCT_H


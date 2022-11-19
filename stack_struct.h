#ifndef STACK_STRUCT_H
#define STACK_STRUCT_H

#include "stack_debug.h"
#include "stack_prot.h"

#define Elem int64_t

struct Stack
{
ON_CANARY_PROT(
    int64_t canary1;
);
    int64_t size;
    int64_t capacity;
    Elem *data;

ON_DEBUG(
    DebugInfo info;
)

ON_HASH_PROT(
    uint64_t xCoeff;
    uint64_t hashData;
    uint64_t hashStk;
)

ON_CANARY_PROT(
    int64_t canary2;
)
};

#endif  // STACK_STRUCT_H

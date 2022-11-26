#ifndef STACK_H
#define STACK_H

#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "stack_struct.h"
#include "stack_prot.h"
#include "stack_debug.h"

#ifdef _DEBUG

#define StackCtor(stk, capacity)    \
StackCtor_(                         \
           stk,                     \
           capacity,                \
           {__LINE__,               \
           __FILE__,                \
           __FUNCTION__,            \
           #stk + (#stk[0] == '&')} \
          )

#else

#define StackCtor(stk, capacity) \
StackCtor_(                      \
           stk,                  \
           capacity              \
          )

#endif

void StackCtor_(
                Stack *stk,
                int64_t capacity 
ON_DEBUG(     , DebugInfo info)
               );

void        StackDtor             (Stack *stk);
void        StackPush             (Stack *stk, Elem el);
Elem        StackTop              (Stack *stk);
Elem        StackPop              (Stack *stk);
void        StackResize           (Stack *stk, int64_t size);
void        StackRealloc          (Stack *stk, int64_t capacity);
uint32_t    StackError            (Stack *stk);
void        StackFillPoison       (Stack *stk, int64_t l, int64_t r);
int64_t     StackGetSize          (Stack *stk);
int64_t     StackGetCapacity      (Stack *stk);
int64_t     StackGetCoeff         (Stack *stk);
bool        isBadPtr              (void *ptr);
ON_CANARY_PROT(
void        StackCanaryUpdate     (Stack *stk);
)
ON_DEBUG(
void        StackDump             (Stack *stk);
int         getfdLogBuffer        ();
void        closeLogBuffer        ();
void        logToBuffer           (const char* event, Stack *stk);
)
ON_HASH_PROT(
uint64_t    StackEvaluateHashData (Stack *stk);
uint64_t    StackEvaluateHashStk  (Stack *stk);
void        StackRehash           (Stack *stk);
)
void        StackDump             (Stack *stk, const char* func, const char* file, long int line);
const char* StackGetStatus        (Stack *stk);
int64_t     max                   (int64_t a, int64_t b);

#endif  // STACK_H

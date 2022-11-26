#include <time.h>
#include <stdlib.h>
#include "stack.h"
#include "stack_struct.h"
#include "stack_debug.h"
#include "stack_prot.h"

ON_DEBUG(
const char* LOG_FILENAME = "stack.log";
)

void StackCtor_(
                Stack *stk,
                int64_t capacity 
ON_DEBUG(     , DebugInfo info)
               )
{
    ASSERT(!isBadPtr(stk));
    Elem* data = (Elem*) calloc(capacity ON_CANARY_PROT(+2), sizeof(Elem));

    ASSERT(!isBadPtr(stk));
    stk->data       = data ON_CANARY_PROT(+1);
    stk->size       = 0;
    stk->capacity   = capacity;
    StackFillPoison (stk, 0, stk->capacity);

ON_CANARY_PROT(
    StackCanaryUpdate(stk);
)

ON_DEBUG(
    stk->info = info;
)

ON_HASH_PROT(
    srand(time(NULL));
    stk->xCoeff = rand() % 519ull;
    StackRehash(stk);
)
}

void StackDtor(Stack *stk)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));

    free(stk->data ON_CANARY_PROT(-1));

    int64_t *ptr = (int64_t*) stk;
    for (int64_t i = 0; i < sizeof(Stack) / sizeof(int64_t); ++i)
        *ptr++ = POISON;

ON_DEBUG
    (
    closeLogBuffer();
    )
}

void StackPush(Stack *stk, Elem el)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));

    StackResize (stk, stk->size + 1);
    stk->data[stk->size - 1] = el;
    
ON_HASH_PROT(
    StackRehash(stk);
)
}

Elem StackTop(Stack *stk)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));
    ASSERT(StackGetSize(stk) > 0);
    
    return stk->data[stk->size - 1];
}

Elem StackPop(Stack *stk)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));
    ASSERT(StackGetSize(stk) > 0);
    
    Elem res = StackTop(stk);
    
    int64_t prevSize = stk->size;
    StackResize (stk, stk->size - 1);
    if (stk->capacity >= prevSize)
        ;//stk->data[stk->size] = POISON;
    
ON_HASH_PROT(
    StackRehash(stk);
)

    return res;
}

void StackResize(Stack *stk, int64_t size)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));

    if (size > stk->capacity)
    {
        int64_t incCoeffCapacity = stk->capacity * StackGetCoeff(stk);
        StackRealloc(stk, max(size, incCoeffCapacity));
    }
    else if (size < stk->size)
    {
        int64_t coeff = StackGetCoeff(stk);
        int64_t decCoeffCapacity = stk->capacity / (coeff * coeff);

        if (size < decCoeffCapacity)
            StackRealloc(stk, decCoeffCapacity);
    }

    stk->size = size;
}

void StackRealloc(Stack *stk, int64_t capacity)
{
    STACK_OK(stk);
    ASSERT(!isBadPtr(stk));

    if (capacity == stk->capacity)
        return;

    Elem *newData = (Elem*) realloc(stk->data ON_CANARY_PROT(-1), sizeof(Elem) * (capacity ON_CANARY_PROT(+2)));
    ASSERT(!isBadPtr(newData));

    stk->data       = newData ON_CANARY_PROT(+1);
    stk->capacity   = capacity;
    StackFillPoison (stk, stk->size, stk->capacity);

ON_CANARY_PROT(
    StackCanaryUpdate(stk);
)
ON_HASH_PROT(
    StackRehash(stk);
)
}

void StackFillPoison(Stack *stk, int64_t l, int64_t r)
{
    ASSERT(!isBadPtr(stk));

    for (int64_t i = l; i < r; ++i)
        ;//stk->data[i] = POISON;

ON_HASH_PROT(
    StackRehash(stk);
)
}

ON_CANARY_PROT(
    static void StackCanaryUpdate(Stack *stk)
    {
        ASSERT(!isBadPtr(stk));
        stk->data[-1]            = CANARY;
        stk->data[stk->capacity] = CANARY;
        stk->canary1             = CANARY;
        stk->canary2             = CANARY;
    ON_HASH_PROT(
        StackRehash(stk);
    )
    }
)

int64_t StackGetSize(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return stk->size;
}

int64_t StackGetCapacity(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return stk->capacity;
}

int64_t StackGetCoeff(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return 2; // TODO: formula
}

const char* StackGetStatus(Stack *stk)
{
    ASSERT(!isBadPtr(stk));

    uint32_t flags = StackError(stk);
    if (flags == 0)
        return "(ok)";

    if (flags & STACK_SIZE_NEG_ERROR)
        return "Stack size is negative";
    if (flags & STACK_CAPACITY_NEG_ERROR)
        return "Stack capacity is negative";
    if (flags & STACK_SIZE_G_CAPACITY_ERROR)
        return "Stack size is greater than capacity";

ON_CANARY_PROT(
    if (flags & STACK_CANARY_OVERWRITE_ERROR)
        return "Stack integrity has been violated, maybe array out of bounds occurred";
)

ON_HASH_PROT(
    if (flags & STACK_HASH_DATA_DISPARITY_ERROR)
        return "Stack integrity has been violated, an arbitrary element of the stack was accessed";
    if (flags & STACK_HASH_STK_DISPARITY_ERROR)
        return "Stack integrity has been violated, stack structure was corrputed";
)

    if (flags & STACK_DATA_POSION_ERROR)
        return "Stack integrity has been violated, unused stack memory accessed";

    if (flags & STACK_DESTROYED_USAGE)
        return "Attempt to reuse a destroyed stack";
}

uint32_t StackError(Stack *stk)
{
    ASSERT(!isBadPtr(stk));

    int64_t *ptr = (int64_t*) stk;
    int64_t cntPoison = 0, size64 = sizeof(Stack) / sizeof(int64_t);
    for (int64_t i = 0; i < size64; ++i)
        cntPoison += *ptr == POISON;

    if (cntPoison > 8 * size64 / 10)
        return STACK_DESTROYED_USAGE;

    uint32_t flags = 0;

    flags |= ((stk->size     < 0)             ? STACK_SIZE_NEG_ERROR         : 0);
    flags |= ((stk->capacity < 0)             ? STACK_CAPACITY_NEG_ERROR     : 0);
    flags |= ((stk->size     > stk->capacity) ? STACK_SIZE_G_CAPACITY_ERROR  : 0);

ON_CANARY_PROT(
    flags |= ((stk->data[-1]            != CANARY ||
               stk->data[stk->capacity] != CANARY ||
               stk->canary1             != CANARY ||
               stk->canary2             != CANARY) ? STACK_CANARY_OVERWRITE_ERROR : 0);
)
    if (flags)
        return flags;

ON_HASH_PROT(
    flags |= (StackEvaluateHashData(stk) != stk->hashData) ? STACK_HASH_DATA_DISPARITY_ERROR : 0;
    flags |= (StackEvaluateHashStk(stk)  != stk->hashStk)  ? STACK_HASH_STK_DISPARITY_ERROR  : 0;
)

    for (int64_t i = stk->size; i < stk->capacity; ++i)
        ;  //flags |= (stk->data[i] != POISON) ? STACK_DATA_POSION_ERROR : 0;

    return flags;
}

ON_DEBUG(
int fdLogBuffer = -1;

int getfdLogBuffer()
{
    if (fdLogBuffer == -1)
    {
        fdLogBuffer = creat(LOG_FILENAME, S_IRWXU);
    }
    return fdLogBuffer;
}

void closeLogBuffer()
{
    close(fdLogBuffer);
    fdLogBuffer = -1;
}
)

void StackDump(Stack *stk, const char* func, const char* file, long int line)
{
}

ON_HASH_PROT(
uint64_t StackEvaluateHashData(Stack *stk)
{
    ASSERT(!isBadPtr(stk));

    uint64_t hashData = 0;
    for (int64_t i = 0; i < stk->capacity; ++i)
        hashData = hashData * stk->xCoeff + stk->data[i];

    return hashData;
}

uint64_t StackEvaluateHashStk(Stack *stk)
{
    ASSERT(!isBadPtr(stk));

    const char *ptr = (const char*) stk;
    uint64_t hashStk = 0;
    for (int64_t i = 0; i < sizeof(Stack) - (2 ON_CANARY_PROT(+1)) * sizeof(uint64_t); ++i)
        hashStk = hashStk * stk->xCoeff + *ptr++;

    return hashStk;
}

void StackRehash(Stack *stk)
{
    ASSERT(!isBadPtr(stk));

    stk->hashData = StackEvaluateHashData(stk);
    stk->hashStk  = StackEvaluateHashStk (stk);
}

)

bool isBadPtr(void *ptr)
{
    if (ptr == NULL)
        return true;

    int nullfd = open("/dev/random", O_WRONLY);
    bool res = write(nullfd, (char*) ptr, 1) < 0;

    close(nullfd);
    return res;
}

int64_t max(int64_t a, int64_t b)
{
    return a > b ? a : b;
}

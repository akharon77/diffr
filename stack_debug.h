#ifndef STACK_DEBUG_H
#define STACK_DEBUG_H

struct DebugInfo
{
    int64_t    line;
    const char* filename;
    const char* funcname;
    const char* name;
};

#define ASSERT(expr)                                        \
do {                                                        \
    if (!(expr))                                            \
        fprintf(stderr,                                     \
                "Assertion failed. Expression: " #expr "\n" \
                "File: %s, line: %d\n"                      \
                "Function: %s\n",                           \
                __FILE__, __LINE__,                         \
                __PRETTY_FUNCTION__);                       \
} while(0)

#ifdef _DEBUG
extern const char* LOG_FILENAME;
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

#define STACK_OK(stk)                                        \
do                                                           \
{                                                            \
    if (StackError(stk) != 0) {                              \
        printf("%s\n", StackGetStatus(stk));                 \
        abort();                                             \
    }                                                        \
    StackDump(stk, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
}                                                            \
while(0)

#endif  // STACK_DEBUG_H

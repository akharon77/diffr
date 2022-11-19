#ifndef STACK_PROT_H
#define STACK_PROT_H

#include <stdint.h>

#ifdef CANARY_PROT
#define ON_CANARY_PROT(...) __VA_ARGS__
#else
#define ON_CANARY_PROT(...)
#endif

#ifdef HASH_PROT
#define ON_HASH_PROT(...) __VA_ARGS__
#else
#define ON_HASH_PROT(...)
#endif

const int64_t POISON = 0xDEADBEEF1451DE1ll;
const int64_t CANARY = 0xBADC0FFEEDED512ll;

const uint32_t STACK_SIZE_G_CAPACITY_ERROR     = 1 << 0;
const uint32_t STACK_SIZE_NEG_ERROR            = 1 << 1;
const uint32_t STACK_CAPACITY_NEG_ERROR        = 1 << 2;
ON_CANARY_PROT(
const uint32_t STACK_CANARY_OVERWRITE_ERROR    = 1 << 3;
)
ON_HASH_PROT(
const uint32_t STACK_HASH_DATA_DISPARITY_ERROR = 1 << 4;
const uint32_t STACK_HASH_STK_DISPARITY_ERROR  = 1 << 5;
)
const uint32_t STACK_DATA_POSION_ERROR         = 1 << 6;
const uint32_t STACK_DESTROYED_USAGE           = 1 << 7;

#endif  // STACK_PROT_H

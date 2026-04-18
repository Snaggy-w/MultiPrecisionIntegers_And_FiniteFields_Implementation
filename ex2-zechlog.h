#ifndef EX2_ZECHLOG_H
#define EX2_ZECHLOG_H
#include "ex2.h"

// Most of the implementation is inspired by AES implementation of logarithm tables:
//      https://crypto.stackexchange.com/a/21174

#define GENERATOR 2 // X (simple). All elements of Field are generators (except 0) -> confirmed by generator.c

void populate_tables();
f128 zech_mul(f128 x, f128 y);
f128 zech_add(f128 x, f128 y);
f128 zech_inverse(f128 x);
f128 zech_division(f128 x, f128 y);

#endif

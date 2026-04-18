#ifndef EX2_H
#define EX2_H

// X^7 + X + 1
#define IRR_POLY (1 << 7) + 2 + 1

typedef unsigned char f128;

f128 add(f128 x, f128 y);
f128 mul(f128 x, f128 y);
f128 inverse(f128 x);
f128 division(f128 x, f128 y);

#endif

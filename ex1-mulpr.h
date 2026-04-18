#ifndef EX1_MULPR_H
#define EX1_MULPR_H

// This implementation idea is not mine!
// 	inspired and adapted from: https://github.com/kokke/tiny-bignum-c/blob/master/bn.h

// As per the license agreement:
/*
 This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
*/

#ifdef MULPR

#include <stdint.h>

// define word size in bytes
#ifndef WORD_SIZE
	#define WORD_SIZE 4 // 32 bits by default
#endif // WORD_SIZE

#define MULPR_ARRAY_SIZE (128/WORD_SIZE) // define a static sized array like the one built-in GCC __uint128_t

/* Here comes the compile-time specialization for how large the underlying array size should be. */
/* The choices are 1, 2 and 4 bytes in size with uint32, uint64 for WORD_SIZE==4, as temporary. */
#ifndef WORD_SIZE
  #error Must define WORD_SIZE to be 1, 2, 4
#elif (WORD_SIZE == 1)
  /* Data type of array in structure */
  #define DTYPE                    uint8_t
  /* bitmask for getting MSB */
  #define DTYPE_MSB                ((DTYPE_TMP)(0x80))
  /* Data-type larger than DTYPE, for holding intermediate results of calculations */
  #define DTYPE_TMP                uint32_t
  /* sprintf format string */
  /* Max value of integer type */
  #define MAX_VAL                  ((DTYPE_TMP)0xFF)
#elif (WORD_SIZE == 2)
  #define DTYPE                    uint16_t
  #define DTYPE_TMP                uint32_t
  #define DTYPE_MSB                ((DTYPE_TMP)(0x8000))
  #define MAX_VAL                  ((DTYPE_TMP)0xFFFF)
#elif (WORD_SIZE == 4)
  #define DTYPE                    uint32_t
  #define DTYPE_TMP                uint64_t
  #define DTYPE_MSB                ((DTYPE_TMP)(0x80000000))
  #define MAX_VAL                  ((DTYPE_TMP)0xFFFFFFFF)
#endif
#ifndef DTYPE
  #error DTYPE must be defined to uint8_t, uint16_t uint32_t or whatever
#endif

typedef struct mulpr{
	DTYPE array[MULPR_ARRAY_SIZE];
}mulpr_t;

// a structure wrapper that supports signs
typedef struct mulpr_signed{
	mulpr_t value;
	int sign; // +1 or -1
}mulpr_signed_t;
// enumerator for comparison function
enum { SMALLER = -1, EQUAL = 0, LARGER = 1 };


void mulpr_init(mulpr_t *n);
void lshift_one_bit(mulpr_t *n);
void rshift_one_bit(mulpr_t *n);
void lshift_word(mulpr_t *n, int nwords);
void rshift_word(mulpr_t *n, int nwords);
void mulpr_assign(mulpr_t *dst, mulpr_t *src);
int mulpr_cmp(mulpr_t *x, mulpr_t *y);
void mulpr_and(mulpr_t *x, mulpr_t *y, mulpr_t *z);
void mulpr_or(mulpr_t *x, mulpr_t *y, mulpr_t *z);
void mulpr_xor(mulpr_t *x, mulpr_t *y, mulpr_t *z);
int mulpr_is_zero(mulpr_t *n);
void mulpr_mul(mulpr_t *x, mulpr_t *y, mulpr_t *z);
void mulpr_from_int(mulpr_t *n, DTYPE_TMP i);
int mulpr_to_int(mulpr_t *n);
void mulpr_add(mulpr_t *x, mulpr_t *y, mulpr_t *z);
void mulpr_sub(mulpr_t *x, mulpr_t *y, mulpr_t *z);
void mulpr_div(mulpr_t *x, mulpr_t *y, mulpr_t *z);
void mulpr_divmod(mulpr_t *x, mulpr_t *y , mulpr_t *z, mulpr_t *w);
void mulpr_mod(mulpr_t *x, mulpr_t *y, mulpr_t *z);
void mulpr_gcd(mulpr_t *x, mulpr_t *y, mulpr_t *g);
void signed_mul_sub(mulpr_signed_t *x, mulpr_signed_t *y, mulpr_t *q, mulpr_signed_t *res);
void mulpr_exgcd(mulpr_t *x, mulpr_t *y, mulpr_signed_t *u, mulpr_signed_t *v, mulpr_t *g);
void mulpr_modexp(mulpr_t *base, mulpr_t *exp, mulpr_t *mod, mulpr_t *res);

#endif // MULPR

#endif //EX1_MULPR_H



#ifndef EX1_H
#define EX1_H

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t Element;

Element add(Element x, Element y, Element n);
Element mul(Element x, Element y, Element n);
Element mul_karatsuba(Element x, Element y, Element n);
Element neg(Element x , Element n);
Element gcd(Element p, Element q, int64_t *u, int64_t *v);
bool isInvertible(Element x, Element n, int64_t *u, int64_t *v);
Element invert(Element x,Element n);
Element modexp(Element x, uint64_t k, Element n);
Element fast_modexp(Element x, uint64_t k, Element n);
#endif // EX1_H

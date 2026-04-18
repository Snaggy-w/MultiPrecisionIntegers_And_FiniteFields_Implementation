#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "ex1.h"

#ifdef MULPR
#include "ex1-mulpr.h"
#endif // MULPR


/*
 * (x + y) mod n
 */
Element add(Element x, Element y, Element n) {
	// reduce x and y to be sure that they will not overflow
	
		
	// x + y > UINT64_MAX -> x > UINT64_MAX - y -> reduce
	while(x > (UINT64_MAX) - y || (UINT64_MAX) - y < UINT64_MAX/2){
		x = ((x >> 1) % n) + ((x >> 1) % n) + (x & 1);
		y = ((y >> 1) % n) + ((y >> 1) % n) + (y & 1);
	}

	
	return (x + y) % n;
}

/* classic multiplication 
 * 	(x * y) mod n
 *
 * This function most likely overflows uint64 space
 *
 * The only posible fix I found to solve the issue was to increase the size to uint128 since mul usually overflows for the given
 * prime numbers. If we cast the multiplication to __uint128_t (https://stackoverflow.com/a/34588884)
 */

/*Element mul(Element x, Element y, Element n){
	
	while(x > (UINT64_MAX) / y || y > (UINT64_MAX) / x){
		x = ((x >> 1) % n) + ((x >> 1) % n) + (x & 1);
		y = ((y >> 1) % n) + ((y >> 1) % n) + (y & 1);
	}
	return (x * y) % n;
}*/

// Same concept of multiplication function discussed with fields
Element mul(Element x, Element y, Element n) {
    Element c = 0; // accumilator
    x = x % n; // reduce x if it is larger than n (just having x as is could overflow since we add x to it later)

    while (y > 0) { // we loop over the bit size of y from msb
        if (y & 1)
            c = (c + x) % n; // mulByX here is just adding another X, we mod to reduce the result below n to avoid overflows again

        x = (x + x) % n; // mulByX again with mod
        y >>= 1; // reduce y
    }

    return c;
}


/*
 * Karatsuba multiplication
 * 	inspired and adapted from: https://gist.github.com/kebunit/4fa83273090837fc1157
 */

int getLength(Element value) {
	int counter = 0;
	while (value != 0) {
		counter++;
		value /= 10;
	}
	return counter;
}

int max(int x, int y){
	return x > y ? x : y;
}

/*
 * in TD1 we looked at karatsuba as so:
 * 	(P0 + XP1)(Q0 + XQ1) = P0Q0 + X((P0 + P1)(Q0 + Q1) − P0Q0 − P1Q1) + X^2 * P1Q1
 */
Element mul_karatsuba(Element x, Element y, Element n){
	int xLen = getLength(x); // determine length to do the split of P: P0 + XP1
	int yLen = getLength(y); // determine length to do the split of Q: Q0 + XQ1

	// get the bigger of the two
	int N = max(xLen,yLen);

	// it's just faster with smaller values
	if(N < 10)
		return mul(x, y, n); 	

	//max length divided and rounded up
	N = (N/2) + (N%2); // Determine the split point of P and Q

	uint64_t multiplier = pow(10 , N); // the actual X multiplied by the upper half
	
	uint64_t b = x/multiplier; // b = upper half of P (or x here): P1
	uint64_t a = x - (b * multiplier); // a = lower half of P (or x here): P0
	uint64_t d = y/multiplier; // d = upper half of Q (or y here): Q1 
	uint64_t c = y - (d * multiplier); // c = lower half of Q (or y here): Q0

	// The three multiplications in the karatsuba algorithm
	Element z0 = mul_karatsuba(a,c,n); // P0.Q0
	Element z1 = mul_karatsuba(a + b, c + d,n); // (P0 + P1)(Q0 + Q1)
	Element z2 = mul_karatsuba(b,d,n); // P1.Q1
	// P0.Q0 + X((P0 + P1)(Q0 + Q1) − P0Q0 − P1Q1) + P1.Q1
	return (z0 + ((z1 - z0 - z2) * multiplier) + (z2 * (uint64_t)(pow(10, 2 * N)))) % n;
}

/*
 * negation: in Z/nZ all elements C -> 0 <= C < n
 * 	     Z/10Z = {0,1,2,3,4,5,6,7,8,9}
 * 	     if we want to get the negation of 4 -> -4 is 10 - 4 mod 10 = 6 mod 10 = 6
 * 	     negation(x) = n - x mod n
 */

Element neg(Element x , Element n){
	return (n - x) % n;
}

/*
 * invertible elements: in Z/nZ, any element x is invertible if GCD(x,n) = 1
 */

/*
 * GCD with bezout coefficients (might be useful for later):
 * 	adapted from: https://stackoverflow.com/questions/59536376/finding-bezout-coefficients-via-extended-euclidean-algorithm-on-array-of-arbitra
 * Recursive GCD implementation is faster than the iterative one presumeably so I am going to use that.
 */

// Source - https://stackoverflow.com/q/59536376
// Posted by cstover, modified by community. See post 'Timeline' for change history
// Retrieved 2026-03-16, License - CC BY-SA 4.0

Element gcd(Element p, Element q, int64_t *u, int64_t *v) {
    if (q == 0){
    	*u = 1;
	*v = 0;
	return p;
    }
    // allocate stack variables for each recursive call
    int64_t u1, v1;

    Element d = gcd(q, p % q, &u1, &v1);
    *u = v1;
    *v = u1 - (p/q) * v1;

    return d;
}

bool isInvertible(Element x, Element n, int64_t *u, int64_t *v){
	return (gcd(x,n,u,v) == 1) ? true : false;
}

/* 
 * Invert: if x is not invertible return 0
 * 	   if x is invertible
 * 	   	GCD(x,n) = 1 -> u.x + v.n = 1 -> u.x = 1 mod n -> x^(-1) = u mod n
 */

Element invert(Element x,Element n){
	int64_t u, v;
	if(!isInvertible(x,n,&u,&v))
		return 0;
	return ((u % (int64_t)n) + n) % n; // this is done to handle possible negative overflow of u
}


/*
 * Modular exponantiation -> iterate over k with multiplications of x
 */

Element modexp(Element x, uint64_t k, Element n){
	Element res = x;
	if(k <= 0)
		return 1;
	for(uint64_t i = 0; i < k - 1; i++)
		res = mul_karatsuba(res, x, n);
	return res;
}

Element fast_modexp(Element x, uint64_t k, Element n){
	Element result = 1;
	x = x % n;

	while(k > 0){
		if(k & 1)
			result = mul(result, x, n);
		x = mul(x,x,n);
		k = k >> 1;
	}

	return result;
}



#ifdef MULPR

// initialize the multiprecision array with zeros
void mulpr_init(mulpr_t *n){
	for(int i = 0; i < MULPR_ARRAY_SIZE; i++){
		n->array[i] = 0;
	}
}

// basic operations on the multipricision

// left shift by one bit

void lshift_one_bit(mulpr_t *n){
	for(int i = MULPR_ARRAY_SIZE - 1; i > 0; i--){
		// shift current word | extracted msb from previous word 
		n->array[i] = (n->array[i] << 1) | (n->array[i - 1] >> ((8 * WORD_SIZE) - 1));
	}
	n->array[0] <<= 1;
}

void rshift_one_bit(mulpr_t *n){
	for(int i = 0; i < MULPR_ARRAY_SIZE - 1; i++){
		// shift current word | extracted lsb from next word
		n->array[i] = (n->array[i] >> 1) | (n->array[i + 1] << ((8 * WORD_SIZE) - 1));
	}
	n->array[MULPR_ARRAY_SIZE - 1] >>= 1;
}


void lshift_word(mulpr_t *n, int nwords){
	int i;
	for(i = MULPR_ARRAY_SIZE - 1; i >= nwords; i--){
		n->array[i] = n->array[i - nwords];
	}
	for(; i >= 0; i--){ // arrays before the shift target should be zero'ed out
		n->array[i] = 0;
	}
}

void rshift_word(mulpr_t *n, int nwords){
	if(nwords >= MULPR_ARRAY_SIZE){ // shift with more than the capacity of the array -> everything is zero
		for(int i = 0; i < MULPR_ARRAY_SIZE; i++){
			n->array[i] = 0;
		}
		return;
	}
	int i;
	for(i = 0; i < MULPR_ARRAY_SIZE - nwords; i++){
		n->array[i] = n->array[i + nwords];
	}
	for(; i < MULPR_ARRAY_SIZE; i++){ // arrays after the right shift should be zero'ed out
		n->array[i] = 0;
	}

}

void mulpr_assign(mulpr_t *dst, mulpr_t *src){
	for(int i = 0; i < MULPR_ARRAY_SIZE; i++){
		dst->array[i] = src->array[i];
	}
}

int mulpr_cmp(mulpr_t *x, mulpr_t *y){

	int i = MULPR_ARRAY_SIZE;
	do{
		i--; // index of last element
		if(x->array[i] > y->array[i])
			return LARGER;
		else if(x->array[i] < y->array[i])
			return SMALLER;
	}while(i != 0);

	return EQUAL;
}

void mulpr_and(mulpr_t *x, mulpr_t *y, mulpr_t *z){
	for(int i = 0; i < MULPR_ARRAY_SIZE; i++){
		z->array[i] = x->array[i] & y->array[i];
	}
}

void mulpr_or(mulpr_t *x, mulpr_t *y, mulpr_t *z){
	for(int i = 0; i < MULPR_ARRAY_SIZE; i++){
		z->array[i] = x->array[i] | y->array[i];
	}
}

void mulpr_xor(mulpr_t *x, mulpr_t *y, mulpr_t *z){
	for(int i = 0; i < MULPR_ARRAY_SIZE; i++){
		z->array[i] = x->array[i] ^ y->array[i];
	}
}

int mulpr_is_zero(mulpr_t *n){
	for(int i = 0; i < MULPR_ARRAY_SIZE; i++){
		if(n->array[i]){
			return 0;
		}
	}
	return 1;
}

// This function is just an adaptation of the uint64 mul function above
void mulpr_mul(mulpr_t *x, mulpr_t *y, mulpr_t *z){
	mulpr_t c,a,b;

	mulpr_init(&c); // c = 0
	mulpr_assign(&a,x);
	mulpr_assign(&b,y);

	while(1){ // b != 0

		// check if b is all zero
		int is_zero = 1;
		for(int i = 0;i < MULPR_ARRAY_SIZE; i++){
			if(b.array[i] != 0){
				is_zero = 0;
				break; // detected no zero word
			}
		}
		if(is_zero){
			mulpr_assign(z, &c);
			return; // b is zero as a whole
		}

		if(b.array[0] & 1){ // lsb of the whole number is 1
			mulpr_t tmp;
			// c = c + x
			mulpr_add(&c, &a, &tmp);
			mulpr_assign(&c,&tmp);
		}
		// mulByX(x) or x = x + x on each iteration
		lshift_one_bit(&a);
		rshift_one_bit(&b);
	}
	mulpr_assign(z, &c); // the result of the accumelator
}



void mulpr_from_int(mulpr_t *n, DTYPE_TMP i){
	mulpr_init(n); // initialize n

	// Most likely the machine will be little-endian, but I kept the coversion anyways as is	
	#ifdef WORD_SIZE
	 // spread the bytes of the integer over the array
	 #if (WORD_SIZE == 1)
	  n->array[0] = (i & 0x000000ff);
	  n->array[1] = (i & 0x0000ff00) >> 8;
	  n->array[2] = (i & 0x00ff0000) >> 16;
	  n->array[3] = (i & 0xff000000) >> 24;
	 #elif (WORD_SIZE == 2)
	  n->array[0] = (i & 0x0000ffff);
	  n->array[1] = (i & 0xffff0000) >> 16;
	 #elif (WORD_SIZE == 4)
	  n->array[0] = i;
	  DTYPE_TMP num_32 = 32;
	  DTYPE_TMP tmp = i >> num_32; /* bit-shift with U64 operands to force 64-bit results */
	  n->array[1] = tmp;
	 #endif
	#endif	
}

int mulpr_to_int(mulpr_t *n){
	int ret = 0;

	#if (WORD_SIZE == 1)
	// construct an integer from each array (also removing a byte from each incrementaly based on the array index) 
	  for(int i = 0; i < 4; i++){
	  	ret += n->array[i] << (8 * i);
	  }
	#elif (WORD_SIZE == 2)
	  for(int i = 0; i < 2; i++){
	  	ret += n->array[i] << (8 * i);
	  }
	#elif (WORD_SIZE == 4)
	  ret += n->array[0];
	#endif

	return ret;
}

// each array represents a multiprecision digit
void mulpr_add(mulpr_t *x, mulpr_t *y, mulpr_t *z){
	
	DTYPE_TMP tmp;

	int carry = 0;
	
	for(int i = 0; i < MULPR_ARRAY_SIZE; i++){
		tmp = (DTYPE_TMP)x->array[i] + y->array[i] + carry;
		carry = (tmp > MAX_VAL); // 1 if there is a carry
		z->array[i] = (tmp & MAX_VAL); // the value without overflows
	}
}

// each number is stored as B = 2^(WORD_SIZE * 8) (MAX_VAL), so we must handle borrows manually
// x[i] - y[i] - borrow could go negative easily so we do:
// 	x - y = (x + B) - (y + borrow)
// if x[i] < y[i] + borrow -> adding B to x avoids going to negative values

void mulpr_sub(mulpr_t *x, mulpr_t *y, mulpr_t *z){
	DTYPE_TMP res;
	DTYPE_TMP tmp1;
	DTYPE_TMP tmp2;

	int borrow = 0;
	for(int i = 0; i < MULPR_ARRAY_SIZE; i++){
		// store the results in DTYPE_TMP = 2*DTYPE which will have room for any overflow and then use that type to detect negative values
		tmp1 = (DTYPE_TMP)x->array[i] + (MAX_VAL + 1); // x[i] + B
		tmp2 = (DTYPE_TMP)y->array[i] + borrow; // y + borrow
		res = (tmp1 - tmp2); // (x + B) - (y + borrow)

		z->array[i] = (DTYPE)(res & MAX_VAL); // res mod B
		borrow = (res <= MAX_VAL); // check if there is a borrow for the next iteration
	}
}

/* division (example):
 *	x = 13, y = 3 -> 13/3 = 4
 *	initialization step:
 *		current = 1
 *		denom = 3
 *		tmp = 13
 *		z = 0
 *	step 1: repeatedly double denom until it exceeds x
 *		iter 1:
 *			denom = 3 <= 13 -> shift
 *			current = 2
 *			denom = 6
 *
 * 		iter 2:
 *			denom = 6 <= 13 -> shift
 *			current = 4
 *			denom = 12
 *		iter 3:
 *			denom = 12 <= 13 -> shift
 *			current = 4
 *			denom = 24
 *		denom = 24 > 13
 *
 *	step 2: step back
 *		current = 4
 *		denom = 12
 *
 *	step 3: building the quotiet z
 *		current = 4
 *		denom = 12
 *		tmp = 13
 *		
 *		tmp > denom -> rmp = tmp - denom = 13 - 12 = 1
 *		z = z or current = 0 or 4 = 4
 *
 *		shift right (reduce current and denom) until tmp < :
 *			iter 1:
 *				current = current >> 1 = 2
 *				denom = 6
 *			iter 2:
 *				current = 2
 *				denom = 3
 *			iter 3:
 *				current = 1
 *				denom = 3
 *				tmp = 1
 *			iter 4:
 *				current = 0 -> we stop
 *			
 *
 */

void mulpr_div(mulpr_t *x, mulpr_t *y, mulpr_t *z){
	mulpr_t current, denom, tmp;

	mulpr_from_int(&current, 1); // current = current power of 2 in the quotient
	mulpr_assign(&denom,y); // denominator
	mulpr_assign(&tmp,x); // tmp = remainder during the division process

	const DTYPE_TMP half_max = 1 + (DTYPE_TMP)(MAX_VAL/2);
	bool overflow = false;
	// while(denom <= x)
	while(mulpr_cmp(&denom, x) != LARGER){
		if(denom.array[MULPR_ARRAY_SIZE - 1] >= half_max){
			overflow = true;
			break;
		}
		lshift_one_bit(&current); // 1 -> 2 -> 4 -> 8... (the power factor) 
		lshift_one_bit(&denom); // y -> 2y -> 4y -> 8y...
	}
	// scale back the denominator in case this division will cause overflow
	if(!overflow){
		rshift_one_bit(&denom); // suppose y = 3 and overflow happens at 24 (8.3) -> scale it back to 12(4.3)
		rshift_one_bit(&current); // scale back current counter accordingly
	}
	mulpr_init(z);
	// while(current != 0) (iterate from largest bit to smallest bit)
	while(!mulpr_is_zero(&current)){
		// if(divident >= denominator)
		// z = sum(2^k) where y.2^k <= remianing value
		if(mulpr_cmp(&tmp, &denom) != SMALLER){
			mulpr_sub(&tmp, &denom, &tmp); // divident = divident - denom
			mulpr_or(z, &current,z); // z = z + current
		}

		rshift_one_bit(&current);
		rshift_one_bit(&denom);
	}
}

//mod: 	mod(a/b) = a - ((a/b) * b)

void mulpr_divmod(mulpr_t *x, mulpr_t *y , mulpr_t *z, mulpr_t *w){
	mulpr_t tmp;
	
	// z = (x/y)
	mulpr_div(x, y, z);

	// tmp = z * y
	mulpr_mul(z,y, &tmp);

	// w = x - tmp
	mulpr_sub(x, &tmp, w);
}

// a mod wrapper
void mulpr_mod(mulpr_t *x, mulpr_t *y, mulpr_t *z){
	mulpr_t tmp;

	mulpr_divmod(x, y, &tmp, z);
}

// similar to the recursive gcd I did above but iterative since it is easier to implement here
void mulpr_gcd(mulpr_t *x, mulpr_t *y, mulpr_t *g){
	mulpr_t tmp_x, tmp_y, r;

	mulpr_assign(&tmp_x,x);
	mulpr_assign(&tmp_y,y);

	while(!mulpr_is_zero(&tmp_y)){
		mulpr_mod(&tmp_x, &tmp_y, &r);
		mulpr_assign(&tmp_x, &tmp_y);
		mulpr_assign(&tmp_y, &r);
	}

	mulpr_assign(g, &tmp_x);
}

// helper for the extended euclidean division to do multiplication and subtraction on signed multipricision numbers

// a - qb = a + (-qb)
void signed_mul_sub(mulpr_signed_t *x, mulpr_signed_t *y, mulpr_t *q, mulpr_signed_t *res){
	mulpr_t qy;
	mulpr_mul(q, &y->value, &qy); // qy = q * abs(y)
	
	// x and y have the same sign
	if(x->sign == y->sign){
		int cmp = mulpr_cmp(&x->value, &qy);
		
		// abs(x) >= qy -> subtraction retains the sign of a
		if(cmp == LARGER || cmp == EQUAL){
			mulpr_sub(&x->value, &qy, &res->value);
			res->sign = x->sign;
		} else { // subtraction flips sign from a
			mulpr_sub(&qy, &x->value, &res->value);
			res->sign = -(x->sign);
		}
	// a: +, b: - -> res = a - (q.b) -> res = abs(a) - (-q.abs(b)) -> res = abs(a) + q.abs(b)  
	} else { // different signs: we just do addition
		mulpr_add(&x->value, &qy, &res->value);
		res->sign = x->sign;
	}

	// if result is 0, make the sign +1 so stuff be predictable
	if(mulpr_is_zero(&res->value)){
		res->sign = 1;
	}

}


// extended gcd with bezout coefficients
// 	split from the other version since this one requires signed multipricision integres wrapper

void mulpr_exgcd(mulpr_t *x, mulpr_t *y, mulpr_signed_t *u, mulpr_signed_t *v, mulpr_t *g){
	mulpr_t r0, r1, q, r;

	mulpr_assign(&r0, x);
	mulpr_assign(&r1, y);

	mulpr_signed_t u0 = { .sign = 1 };
	mulpr_signed_t u1 = { .sign = -1 };

	mulpr_from_int(&u0.value, 1); // u0 = 1
	mulpr_init(&u1.value); // u1 = 0
	
	mulpr_signed_t v0 = { .sign = 0 };
	mulpr_signed_t v1 = { .sign = 1 };

	mulpr_init(&v0.value); // v0 = 0
	mulpr_from_int(&v1.value, 1); // v1 = 1
	
	while(!mulpr_is_zero(&r1)){
	
		mulpr_div(&r0, &r1, &q); // q = r0 / r1
	
		// r = r0 - q*r1
		mulpr_t tmp;
		mulpr_mul(&q,&r1, &tmp);
		mulpr_sub(&r0, &tmp, &r);

		// shuffle r's: ri -> ri+1 
		mulpr_assign(&r0,&r1);
		mulpr_assign(&r1,&r);
		
	        // u = u0 - q*u1
		mulpr_signed_t new_u;
		signed_mul_sub(&u0, &u1, &q, &new_u);
		
		// ui -> ui+1
		u0 = u1;
		u1 = new_u;

		// v = v0 - q*v1
		mulpr_signed_t new_v;
		signed_mul_sub(&v0, &v1, &q, &new_v);

		// vi -> vi+1
		v0 = v1;
		v1 = new_v;	
	}

	*u = u0;
	*v = v0;
	mulpr_assign(g, &r0);
}

// exactly the fast_modexp function above nut using the mulpr functions
void mulpr_modexp(mulpr_t *base, mulpr_t *exp, mulpr_t *mod, mulpr_t *res){
	mulpr_t x,e;

	mulpr_assign(&x, base);
	mulpr_assign(&e,exp);

	mulpr_from_int(res, 1);
	
	// while k > 0
	while(!mulpr_is_zero(&e)){
		if(e.array[0] & 1){ // if k & 1
			// res = (res * x) % n
			mulpr_t tmp;
			mulpr_mul(res, &x, &tmp);
			mulpr_mod(&tmp, mod, res);
		}
		
		// x = (x*x) %n
		mulpr_t tmp;
		mulpr_mul(&x, &x, &tmp);
		mulpr_mod(&tmp, mod, &x);
		// k >> 1
		rshift_one_bit(&e);
	}
}

#endif // MULPR

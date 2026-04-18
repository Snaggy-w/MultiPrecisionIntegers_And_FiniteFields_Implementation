#include <stdlib.h>
#include <stdint.h>
#include "ex2.h"


// addition: X XOR Y
// also subtraction
f128 add(f128 x, f128 y){
	return (x ^ y) & 0x7F;
}


// multiply: mulByX iteratively
f128 mulByX(f128 x){
	f128 b = x << 1;
	if(x & 64) 
		b ^= IRR_POLY;
	return b & 0x7F; // in all returned values, I will mask with 0x7f (0b01111111) to bind it as 2^7 field and ignore the leftover overflowed values
}

f128 mul(f128 x, f128 y){
	f128 c = 0; // accumilator
	f128 g = x;
	for(int i = 0; i < 7; i++){
		if(y & 1)
			c ^= g;
		g = mulByX(g);
		y = y >> 1;
	}
	return c & 0x7F;
}


// inverse operation via extended euclidean division


int msb(f128 x){
	f128 bitSize = 8;
	f128 current_index = 0;

	if(x == 0)
		return -1;

	while(current_index < bitSize){
		if(x & 0x80) // check the bit at X^7 if 1 -> stop and that's our degree gotten from current_index
			break;
		x = x << 1; // left shift to push the degree to the left
		current_index++; // increase the index (shifts counter)
	}

	return bitSize - (current_index + 1); // subtract the bit size from how many shifts we did = degree 

}



f128 inverse(f128 x){

	if(x == 0) return 0;	
	// I decided to use larger size (uint64) for the coefficients as they need to hold the irreducible polynomial and a smaller size could result 
	// quickly in 
	uint64_t r0 = IRR_POLY;
	uint64_t r1 = x;

	uint64_t u0 = 0, u1 = 1;
	uint64_t v0 = 1, v1 = 0;

	// inspired from: https://stackoverflow.com/a/70501399
	
	
	while(r1 != 0){
		int deg_r0 = msb(r0);
		int deg_r1 = msb(r1);
		
		// q = X^shift
		int shift = deg_r0 - deg_r1; // q = r0/r1 to get the degree difference and shift with that later
		
		// if deg(r0) < deg(r1) we swap the roles of the attributes
		if(shift < 0){
			uint64_t tmp;
			tmp = r0;r0 = r1;r1 = tmp;
			tmp = v0;v0 = v1;v1 = tmp;
			tmp = u0;u0 = u1;u1 = tmp;
			//deg_r0 = msb(r0);
			//deg_r1 = msb(r1);
			continue; // skip the iteration to redo the calculation of correct order of coefficients (higher to lower degree)
		}

		// r0 = ri - 1; r1 = ri -> r0(updated) = r(i -1) - ri*q -> r0 = r0 XOR r1 * q(shift)
		r0 ^= r1 << shift;
		// same concept for u and v
		u0 ^= u1 << shift;
		v0 ^= v1 << shift;
	}

	// inverse is u0 at the end
	return (f128)(u0 & 0x7F);

}

// division: a/b = a * b^-1

f128 division(f128 x, f128 y){
	return mul(x, inverse(y));
}


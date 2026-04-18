#include "ex1.h"
#include "rsa.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

rsa_key keygen(Element p, Element q){
	rsa_key key = {0};
	
	// n = p.q
	key.n = p * q;

	// https://crypto.stackexchange.com/a/76260
	key.e = 65537;

	//phi(n) = (p-1)(q-1) 
	//Element phi = mul_karatsuba((p - 1),(q - 1),key.n);
	Element phi = (p - 1) * (q - 1); 
	printf("phi = %lu\n", phi);
	int64_t u,v;
	if(!isInvertible(key.e,phi,&u,&v)){
		perror("Choose compatible prime numbers!");
		exit(1);
	}

	key.d = invert(key.e,phi);
        printf("e*d mod phi = %lu\n", mul(key.e,key.d,phi));

	return key;
}


Element Enc(Element m, Element e,Element n){
	return fast_modexp(m,e,n);
}

Element Dec(Element c, Element d, Element n){
	return fast_modexp(c,d,n);
}


#ifdef MULPR

rsa_key_mulpr keygen_mulpr(mulpr_t *p, mulpr_t *q){
	rsa_key_mulpr key;

	mulpr_t p1, q1, phi, tmp;
	
	// n = p * q
	mulpr_mul(p,q, &key.n);

	// same value used in the 64 bit implementation for e
	mulpr_from_int(&key.e, 65537);
	
	// p1 = p -1 and q1 = q - 1, tmp = 1
	mulpr_from_int(&tmp,1);
	mulpr_sub(p,&tmp,&p1);
	mulpr_sub(q,&tmp,&q1);
	// phi = p1 * q1
	mulpr_mul(&p1,&q1,&phi);

	// doing extended gcd to check invertibility
	mulpr_signed_t u,v;
	mulpr_t g;

	mulpr_exgcd(&key.e,&phi,&u,&v,&g);
	// tmp = 1 from above
	if(mulpr_cmp(&g,&tmp) != EQUAL){
		printf("Choose compatible prime numbers!\n");
		exit(1);
	}

	// d = e^-1 mod phi = u mod phi (u calculated by executing exgcd)
	// we just need to take care of the sign of u since it could be negative (we just wrap it around with phi)
	if(u.sign < 0){
		// d = u + phi
		mulpr_add(&u.value, &phi, &key.d);
	} else {
		mulpr_assign(&key.d, &u.value); // d = u normally
	}

	return key;
	
}

void Enc_mulpr(mulpr_t *m, mulpr_t *e, mulpr_t *n, mulpr_t *res){
	mulpr_modexp(m , e, n, res);
}

void Dec_mulpr(mulpr_t *c, mulpr_t *d, mulpr_t *n, mulpr_t *res){
	mulpr_modexp(c, d, n, res);
}

#endif // MULPR

#include <stdio.h>
#include "ex1.h"
#include <stdint.h>
#include <stdlib.h>
#include "ex1-mulpr.h"

int main() {

    // init of the values globally
    Element n = 17;
    Element x = 5;
    Element y = UINT64_MAX/2 + 11; //3;

    // for modular exponantiation
    uint64_t k = 5;

    printf("Working in Z/%luZ\n\n", n);


    Element a = add(x, y, n);
    printf("add(%lu,%lu) mod %lu = %lu\n", x, y, n, a);

    Element m = mul(x, y, n);
    printf("mul(%lu,%lu) mod %lu = %lu\n", x, y, n, m);

    Element mk = mul_karatsuba(x, y, n);
    printf("mul_karatsuba(%lu,%lu) mod %lu = %lu\n", x, y, n, mk);

    Element negx = neg(x, n);
    printf("neg(%lu) mod %lu = %lu\n", x, n, negx);

    int64_t u, v;
    Element d = gcd(x, n, &u, &v);
    printf("gcd(%lu,%lu) = %lu\n", x, n, d);
    printf("Bezout coefficients: %lu*%ld + %lu*%ld = %lu\n", x, (long)u, n, (long)v, d);

    if(isInvertible(x, n, &u, &v))
        printf("%lu is invertible mod %lu\n", x, n);
    else
        printf("%lu is NOT invertible mod %lu\n", x, n);

    Element inv = invert(x, n);
    if(inv != 0)
        printf("inverse(%lu) mod %lu = %lu\n", x, n, inv);
    else
        printf("%lu has no inverse mod %lu\n", x, n);

    Element e = modexp(x, k, n);
    printf("%lu^%lu mod %lu = %lu\n", x, k, n, e);
    Element ef = fast_modexp(x,k,n);
    printf("%lu^%lu mod %lu (fast mod exp) = %lu\n", x, k, n, ef);

#ifdef MULPR

    printf("Multiprecision tests:\n");

    mulpr_t mx, my, mn, mres;
    mulpr_from_int(&mx, x);
    mulpr_from_int(&my, y);
    mulpr_from_int(&mn, n);

    // mulpr addition
    mulpr_add(&mx, &my, &mres);
    printf("mulpr_add(%lu,%lu) = %d\n", x, y, mulpr_to_int(&mres));

    // mulpr subtraction
    mulpr_sub(&mx, &my, &mres);
    printf("mulpr_sub(%lu,%lu) = %d\n", x, y, mulpr_to_int(&mres));

    // mulpr multiplication
    mulpr_mul(&mx, &my, &mres);
    printf("mulpr_mul(%lu,%lu) = %d\n", x, y, mulpr_to_int(&mres));

    // mulpr division
    mulpr_div(&mx, &my, &mres);
    printf("mulpr_div(%lu,%lu) = %d\n", x, y, mulpr_to_int(&mres));

    // mulpr modulo
    mulpr_mod(&mx, &my, &mres);
    printf("mulpr_mod(%lu,%lu) = %d\n", x, y, mulpr_to_int(&mres));

    // regular gcd 
    mulpr_t mg;
    mulpr_gcd(&mx, &my, &mg);
    printf("mulpr_gcd(%lu,%lu) = %d\n", x, y, mulpr_to_int(&mg));

    // extended gcd with bezout coefiicients
    mulpr_signed_t mu, mv;
    mulpr_exgcd(&mx, &my, &mu, &mv, &mg);

    printf("mulpr_exgcd(%lu,%lu):\n", x, n);
    printf("gcd = %d\n", mulpr_to_int(&mg));
    printf("u = %s%d\n", (mu.sign < 0 ? "-" : ""), mulpr_to_int(&mu.value));
    printf("v = %s%d\n", (mv.sign < 0 ? "-" : ""), mulpr_to_int(&mv.value));

    // check if gcd = u*x + v*y
    //int ux = mulpr_to_int(&mu.value) * (mu.sign);
    //int vn = mulpr_to_int(&mv.value) * (mv.sign);
    //printf("u*x + v*y = %ld\n", ux * x + vn * y);
    
    mulpr_t temp1,temp2;
    
    //mulpr_from_int(&temp,ux * x + vn * y);
    //mulpr_mul(&mu.value, &mx, &temp1); // ux + x
    //mulpr_mul(&mv.value, &my, &temp2);
    //mulpr_add(&temp1,&temp2,&temp);
    


    mulpr_signed_t ux, vy, result;
    
    // ux = u * x
    mulpr_mul(&mu.value, &mx, &temp1);
    ux.value = temp1;
    ux.sign = mu.sign;
    
    // vy = v * y
    mulpr_mul(&mv.value, &my, &temp2);
    vy.value = temp2;
    vy.sign = mv.sign;
    
    // result = ux + vy
    if (ux.sign == vy.sign) { // we add on equality 
        mulpr_add(&ux.value, &vy.value, &result.value);
        result.sign = ux.sign;
    } else { // sub on sign difference
        int cmp = mulpr_cmp(&ux.value, &vy.value);
        if (cmp == LARGER) {
            mulpr_sub(&ux.value, &vy.value, &result.value);
            result.sign = ux.sign;
        } else {
            mulpr_sub(&vy.value, &ux.value, &result.value);
            result.sign = vy.sign;
        }
    }

    if(mulpr_cmp(&mg,&result.value) != EQUAL){
    	printf("gcd not equal to u*x + v*y!");
	exit(1);
    } else {
    	printf("gcd(x,y) and ux + vy are equal!\n");
    }

    // mulpr modular exponantiation
    mulpr_t mk_exp, mk_base, mk_mod, mk_res;
    mulpr_from_int(&mk_base, x);
    mulpr_from_int(&mk_exp, k);
    mulpr_from_int(&mk_mod, n);

    mulpr_modexp(&mk_base, &mk_exp, &mk_mod, &mk_res);
    printf("mulpr_modexp(%lu,%lu) mod %lu = %d\n", x, k, n, mulpr_to_int(&mk_res));

#endif // MULPR

    return 0;
}

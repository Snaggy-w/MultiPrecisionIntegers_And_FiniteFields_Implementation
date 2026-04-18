#include <stdio.h>
#include <stdlib.h>
#include "ex2.h"
#include "ex2-zechlog.h"

int main() {
    // same values in f128-demo for comparison
    f128 a = 0x11;//0x12; // X^4 + X
    f128 b = 0x04;//0x05; // X^2 + 1

    // init the tables
    populate_tables();

    // bitwise operations in F128
    f128 sum1 = add(a, b);
    f128 product1 = mul(a, b);
    f128 a_inv1 = inverse(a);
    f128 b_inv1 = inverse(b);
    f128 div1 = division(a, b);

    printf("Bitwise F128 operations:\n");
    printf("a + b = 0x%02X\n", sum1);
    printf("a * b = 0x%02X\n", product1);
    printf("a^-1 = 0x%02X\n", a_inv1);
    printf("b^-1 = 0x%02X\n", b_inv1);
    printf("a / b = 0x%02X\n", div1);
    printf("a * a^-1 = 0x%02X\n", mul(a, a_inv1));
    printf("b * b^-1 = 0x%02X\n\n", mul(b, b_inv1));

    // zech-log operations in field F128
    f128 sum2 = zech_add(a, b);
    f128 product2 = zech_mul(a, b);
    f128 a_inv2 = zech_inverse(a);
    f128 b_inv2 = zech_inverse(b);
    f128 div2 = zech_division(a, b);

    printf("Zech-log in F128 operations:\n");
    printf("a + b = 0x%02X\n", sum2);
    printf("a * b = 0x%02X\n", product2);
    printf("a^-1 = 0x%02X\n", a_inv2);
    printf("b^-1 = 0x%02X\n", b_inv2);
    printf("a / b = 0x%02X\n", div2);
    printf("a * a^-1 = 0x%02X\n", zech_mul(a, a_inv2));
    printf("b * b^-1 = 0x%02X\n", zech_mul(b, b_inv2));

    // compare the results of the two versions
    if(sum1 == sum2) 
	    printf("\nAddition matches!\n");
    else{
	    printf("\nAddition mismatch!\n");
    	    exit(1);
    }

    if(product1 == product2) 
	    printf("Multiplication matches!\n");
    else{
	    printf("Multiplication mismatch!\n");
    	    exit(1);
    }

    if(a_inv1 == a_inv2 && b_inv1 == b_inv2) 
	    printf("Inverses match!\n");
    else{
	    printf("Inverse mismatch!\n");
    	    exit(1);
    }

    if(div1 == div2) 
	    printf("Division matches!\n");
    else{
	    printf("Division mismatch!\n");
    	    exit(1);
    }

    return 0;
}

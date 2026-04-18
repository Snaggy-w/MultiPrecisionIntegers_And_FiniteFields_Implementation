#include <stdio.h>
#include "ex2.h"

int main() {
    f128 a = 0x12; // 0b00010010 = X^4 + X
    f128 b = 0x05; // 0b00000101 = X^2 + 1

    f128 sum = add(a, b);
    printf("a + b = 0x%02X\n", sum);

    f128 product = mul(a, b);
    printf("a * b = 0x%02X\n", product);

    f128 a_inv = inverse(a);
    printf("a^-1 = 0x%02X\n", a_inv);
    
    f128 b_inv = inverse(b);
    printf("b^-1 = 0x%02X\n", b_inv);

    f128 div_ab = division(a, b);
    printf("a / b = 0x%02X\n", div_ab);

    f128 check = mul(a, a_inv);
    if(check != 1){ // it should essentially produce a/a = 1
	    printf("ERROR: a * a^-1 != 1\n");
	    printf("a * a^-1 = 0x%02X\n", check);
	    return 1;
    }
    
    return 0;
}

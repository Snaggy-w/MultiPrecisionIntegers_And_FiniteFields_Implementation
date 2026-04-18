#include <stdio.h>
#include "ex2.h"


// inspired by:
// 	https://stackoverflow.com/a/62640210

// order of an element is the smallest integer n s.t a^n = 1
f128 order(f128 a){
	if(a == 0)
		return 0;

	f128 cnt = 0;
	f128 m = 1;

	do{
		cnt++; // count how many times we multiply a by itself (powers) until we reach 1
		m = mul(m, a);

	}while(m != 1);

	return cnt;
}

// a generator is an element of order 127 in F128
f128 find_generators(f128 *gen){

	int count = 0;
	for(int i = 1; i < 128; i++){ // 1 to 127 (skip 0 and 128)
		f128 ord_i = order((f128)i);

		if(ord_i == 127){
			gen[count++] = (f128)i;
		}

	}
	return count;
}

int main(){
    f128 gens[127];

    int n = find_generators(gens);

    printf("Generators: [");
    for(int i = 0; i < n; i++){
	if(i != n - 1)
        	printf("0x%02X,", gens[i]);
	else
		printf("0x%02X]", gens[i]);
    }
    printf("\n");

    printf("Total: %d\n", n);

    return 0;
}

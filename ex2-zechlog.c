#include "ex2.h"
#include "ex2-zechlog.h"
// Most of the implementation is inspired by AES implementation of logarithm tables:
// 	https://crypto.stackexchange.com/a/21174


static f128 f128_exp_table[128];
static f128 f128_log_table[128];
static f128 zech_addition_table[128];

void populate_tables(){
	f128 t = 1;
	for(int i = 0; i < 127; i++){
		f128_exp_table[i] = t;		
		t = mul(GENERATOR, t); // g^i
	}
	for(int i = 1; i < 127; i++){ // log(g^i) = i
		f128_log_table[f128_exp_table[i]] = (f128)i;
	}
	// log(0) an undefined case
	f128_log_table[0] = 127;

	// 1 + g^i -> save them for use in addition
	for(int i = 0; i < 127; i++){
		f128 val = add(f128_exp_table[i],1);
		if(val == 0)
			zech_addition_table[i] = 127; // special case if value is 0 (since we will do log)
		else
			zech_addition_table[i] = f128_log_table[val];
	}
}

// a x b = g^i x g^j = g^(i+j)
f128 zech_mul(f128 x, f128 y){
	if(x == 0 || y == 0)
		return 0;
	f128 i = f128_log_table[x];
	f128 j = f128_log_table[y];

	return f128_exp_table[(i + j) % 127];	
}

// a + b = g^i x (1 + g^(j - i)) = g^i x g^zech(j - i) = g^(i + zech(j - i))
f128 zech_add(f128 x, f128 y){
	if(x == 0)
		return y;
	if(y == 0)
		return x;

	f128 i = f128_log_table[x];
	f128 j = f128_log_table[y];

	if(i > j){ // swap to ensure order
		f128 tmp;
		tmp = i;
		i = j;
		j = tmp;
	}

	f128 z = zech_addition_table[j - i]; // zech-log: log(1 + g^(j-i))
	if(z == 127)
		return 0; // edge case where 1 + g^(j-i) = 0
	return f128_exp_table[(i + z) % 127]; // g^(i + zech(j - i))
}

// inverse: (g^i)^-1 = g^(127-i)

f128 zech_inverse(f128 x){
	if(x == 0)
		return 0;
	f128 i = f128_log_table[x];
	return f128_exp_table[(127 - i) % 127];
}

// division: g^i/g^j = g^(i - j) = g^(i - j + 127) % 127 

f128 zech_division(f128 x, f128 y){
	if(x == 0)
		return 0;
	f128 i = f128_log_table[x];
	f128 j = f128_log_table[y];
	return f128_exp_table[(i - j + 127) % 127];
}


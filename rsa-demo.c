#include <stdio.h>
#include <stdlib.h>
#include "ex1.h"
#include "rsa.h"



int main() {

    Element p = 804582047; //313486403;
    Element q = 981009509; //521502889;

    printf("Using primes:\n");
    printf("p = %lu\n", p);
    printf("q = %lu\n\n", q);

    rsa_key key = keygen(p,q);

    printf("Generated RSA parameters:\n");
    printf("n = %lu\n", key.n);
    printf("e = %lu\n", key.e);
    printf("d = %lu\n\n", key.d);

    
    Element message = 123456;

    printf("Original message: %lu\n", message);

    Element ciphertext = Enc(message, key.e, key.n);
    printf("Encrypted message: %lu\n", ciphertext);

    Element decrypted = Dec(ciphertext, key.d, key.n);
    printf("Decrypted message: %lu\n", decrypted);

    if(decrypted == message)
        printf("Success: decrypted message matches original.\n");
    else {
        printf("Error: decryption failed.\n");
	exit(1);
    }

#ifdef MULPR
#include "ex1-mulpr.h"

    printf("\nAttempting rsa with multiprecision integers implementation:\n");
    mulpr_t p1,q1; 
    mulpr_from_int(&p1,313486403);
    mulpr_from_int(&q1,521502889);

    printf("Using primes:\n");
    printf("p = %u\n", mulpr_to_int(&p1));
    printf("q = %u\n\n", mulpr_to_int(&q1));

    rsa_key_mulpr key1 = keygen_mulpr(&p1,&q1);

    printf("Generated RSA parameters:\n");
    printf("n = %u\n", mulpr_to_int(&key1.n));
    printf("e = %u\n", mulpr_to_int(&key1.e));
    printf("d = %u\n\n", mulpr_to_int(&key1.d));

    
    mulpr_t message1;
    mulpr_from_int(&message1,123456);
    printf("Original message: %u\n", mulpr_to_int(&message1));

    mulpr_t ciphertext1;
    Enc_mulpr(&message1, &key1.e, &key1.n, &ciphertext1);
    
    printf("Encrypted message: %u\n", mulpr_to_int(&ciphertext1));
    
    mulpr_t decrypted1;
    Dec_mulpr(&ciphertext1, &key1.d, &key1.n,&decrypted1);
    printf("Decrypted message: %u\n", mulpr_to_int(&decrypted1));
    
    if(mulpr_cmp(&message1, &decrypted1) == EQUAL)
        printf("Success: decrypted message matches original.\n");
    else {
        printf("Error: decryption failed.\n");
	exit(1);
    }

#endif // MULPR
    return 0;
}

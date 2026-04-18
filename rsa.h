#include "ex1.h"

typedef struct {
	Element e;
	Element n;
	Element d;
} rsa_key;

rsa_key keygen(Element p, Element q);
Element Enc(Element m, Element e,Element n);
Element Dec(Element c, Element d, Element n);


#ifdef MULPR

#include "ex1-mulpr.h"

typedef struct {
	mulpr_t n;
	mulpr_t e;
	mulpr_t d;
} rsa_key_mulpr;

rsa_key_mulpr keygen_mulpr(mulpr_t *p, mulpr_t *q);
void Enc_mulpr(mulpr_t *m, mulpr_t *e, mulpr_t *n, mulpr_t *res);
void Dec_mulpr(mulpr_t *c, mulpr_t *d, mulpr_t *n, mulpr_t *res);
#endif // MULPR

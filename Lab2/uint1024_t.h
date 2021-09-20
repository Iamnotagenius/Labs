#include <stdint.h>
#define UINT1024_SIZE 128
#define BASE 256
typedef struct {
	uint8_t digit[UINT1024_SIZE];
} uint1024_t;

typedef struct {
	uint1024_t *quot;
	uint1024_t *rem;
} uint1024_div;

uint1024_t *uint1024_from_uint(unsigned int x);
int compare(uint1024_t *x, uint1024_t *y);
uint1024_t *add(uint1024_t *x, uint1024_t *y);
void ladd(uint1024_t *x, uint1024_t *y);
void inc(uint1024_t *x);
uint1024_t *substract(uint1024_t *x, uint1024_t *y);
void lsubstract(uint1024_t *x, uint1024_t *y);
void dec(uint1024_t *x);
uint1024_t *mult(uint1024_t *x, uint1024_t *y);
void lmult(uint1024_t *x, uint1024_t *y);
uint1024_div *divmod(uint1024_t *dividend, uint1024_t *divisor);
char *to_str(const uint1024_t *x);
void scanf_uint1024(char *format, uint1024_t *x);

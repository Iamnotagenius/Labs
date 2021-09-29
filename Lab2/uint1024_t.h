#include <stdint.h>
#define UINT1024_MIN_SIZE 32
#define BASE 100 /* each digit in following struct is a 2-digit decimal number */
typedef struct {
	uint8_t *digit;
	uint32_t size;
} uint1024_t;
/* digits are located in memory from lower to higher */

typedef struct {
	uint1024_t quot;
	uint1024_t rem;
} uint1024_div;

uint1024_t uint1024_from_uint(unsigned int x);

int compare(uint1024_t x, uint1024_t y);

/* equivalent of '+' operator */
uint1024_t add(uint1024_t x, uint1024_t y);
/* equivalent of '+=' operator */
void ladd(uint1024_t x, uint1024_t y);
/* equivalent of '++' operator */
void inc(uint1024_t x);

/* equivalent of '-' operator */
uint1024_t substract(uint1024_t x, uint1024_t y);
/* equivalent of '-=' operator */
void lsubstract(uint1024_t x, uint1024_t y);
/* equivalent of '--' operator */
void dec(uint1024_t x);

/* equivalent of '*' operator */
uint1024_t mult(uint1024_t x, uint1024_t y);
/* equivalent of '*=' operator */
void lmult(uint1024_t x, uint1024_t y);

/* computes quotient and remainder of divison */
uint1024_div divmod(uint1024_t dividend, uint1024_t divisor);
/* similar to previous function, but it does not allocates memory without freeing it */
void ldivmod(uint1024_t dividend, uint1024_t divisor, uint1024_t mod);

/* equivalent of '/' operator */
uint1024_t divide(uint1024_t dividend, uint1024_t divisor);
/* equivalent of '/=' operator */
void ldivide(uint1024_t dividend, uint1024_t divisor);

/* equivalent of '%' operator */
uint1024_t mod(uint1024_t dividend, uint1024_t divisor);
/* equivalent of '%=' operator */
void lmod(uint1024_t dividend, uint1024_t divisor);

/* makes string representaion */
char *to_str(uint1024_t x);
uint1024_t scan_uint1024(char *str);

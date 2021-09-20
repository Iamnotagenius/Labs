#include <stdint.h>
#define UINT1024_SIZE 128
#define BASE 256
typedef struct {
	uint8_t *digit;
} uint1024_t;

uint1024_t uint1024_from_uint(unsigned int x);
uint1024_t add(uint1024_t *x, uint1024_t *y);
uint1024_t substract(uint1024_t *x, uint1024_t *y);
uint1024_t mult(uint1024_t *x, uint1024_t *y);
char *to_str(const uint1024_t *x);
void scanf_uint1024(char *format, uint1024_t *x);

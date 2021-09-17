#include <stdlib.h>
#include "uint1024_t.h"

uint1024_t uint1024_from_uint(unsigned int x) {
	uint1024_t value;
	int i = 0;	
	while (x) {
		value.digit[i] = x % 256;
		x /= 256;
		i++;
	}
	return value;
}

char *to_str(uint1024_t x) {
	char *str = malloc(
	

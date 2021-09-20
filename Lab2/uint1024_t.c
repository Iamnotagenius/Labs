#include <stdio.h>
#include <stdlib.h>
#include "uint1024_t.h"

int count_significant_digits(const uint1024_t *x) {
	int i = UINT1024_SIZE - 1;
	while (x->digit[i] == 0)
		i--;
	return i + 1;
}

uint1024_t uint1024_from_uint(unsigned int x) {
	uint1024_t value;
	int i = 0;	
	while (x) {
		value.digit[i] = x % BASE;
		x /= BASE;
		i++;
	}
	return value;
}

char *to_str(const uint1024_t *x) {
	int current_digit, significant_digits;
	int length = count_significant_digits(x);
	char *str = malloc(3 * length);
	for (int i = 0; i < length; i++)
		sprintf(str, "%x ", x->digit[i]);
	str[3 * length - 1] = '\0';
	return str;
}

#include <stdbool.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include "uint1024_t.h"

int count_significant_digits(const uint1024_t *x) {
	int i = UINT1024_SIZE - 1;
	while (x->digit[i] == 0 and i > 0)
		i--;
	return i;
}

int max_digits(uint1024_t *x, uint1024_t *y) {
	int i = UINT1024_SIZE - 1;
	while (x->digit[i] == 0 and y->digit[i] == 0)
		i--;
	return i;
}

uint1024_t uint1024_from_uint(unsigned int x) {
	uint1024_t value;
	value.digit = calloc(UINT1024_SIZE, sizeof(uint8_t));
	int i = 0;
	while (x) {
		value.digit[i] = x % BASE;
		x /= BASE;
		i++;
	}
	return value;
}

uint1024_t add(uint1024_t *x, uint1024_t *y) {
	uint1024_t result;
	result.digit = calloc(UINT1024_SIZE, sizeof(uint8_t));
	bool overflow = false;
	int significant = max_digits(x, y);
	for (int i = 0; i <= significant; i++) {
		result.digit[i] = x->digit[i] + y->digit[i] + overflow;
		overflow = x->digit[i] > result.digit[i]; 
	}
	if (overflow and significant < UINT1024_SIZE - 1)
		result.digit[significant + 1] = 1;

	return result;
}

char *to_str(const uint1024_t *x) {
	int current_digit, significant_digits;
	int length = count_significant_digits(x);
	char *str = malloc(3 * (length + 1));
	for (int i = 0; i <= length; i++)
		sprintf(str + 3 * i, "%02x ", x->digit[i]);
	str[3 * (length + 1) - 1] = '\0';
	return str;
}

#include <stdbool.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include "uint1024_t.h"

uint1024_t *init() {
	return calloc(UINT1024_SIZE, sizeof(uint8_t));
}

uint1024_t *copy(uint1024_t *x) {
	uint1024_t *new = malloc(sizeof(uint1024_t));
	for (int i = 0; i < UINT1024_SIZE; i++)
		new->digit[i] = x->digit[i];
	return new;
}

void copyto(uint1024_t *src, uint1024_t *dest) {
	for (int i = 0; i < UINT1024_SIZE; i++)
		dest->digit[i] = src->digit[i];
}

int count_significant_digits(const uint1024_t *x) {
	int i = UINT1024_SIZE - 1;
	while (x->digit[i] == 0 and i > 0)
		i--;
	return i;
}

int max_digits(uint1024_t *x, uint1024_t *y) {
	int i = UINT1024_SIZE - 1;
	while (x->digit[i] == 0 and y->digit[i] == 0 and i > 0)
		i--;
	return i;
}

uint1024_t *uint1024_from_uint(unsigned int x) {
	uint1024_t *value = init();
	int i = 0;
	while (x) {
		value->digit[i] = x % BASE;
		x /= BASE;
		i++;
	}
	return value;
}

int compare(uint1024_t *x, uint1024_t *y) {
	for (int i = max_digits(x, y); i >= 0; i--) {
		if (x->digit[i] > y->digit[i])
			return 1;
		if (x->digit[i] < y->digit[i])
			return -1;
	}
	return 0;
}

uint1024_t *add(uint1024_t *x, uint1024_t *y) {
	uint1024_t *result = init();
	bool overflow = false;
	int significant = max_digits(x, y);
	for (int i = 0; i <= significant; i++) {
		result->digit[i] = x->digit[i] + y->digit[i] + overflow;
		overflow = x->digit[i] > result->digit[i]; 
	}
	if (overflow and significant < UINT1024_SIZE - 1)
		result->digit[significant + 1] = 1;

	return result;
}

void ladd(uint1024_t *x, uint1024_t *y) {
	bool overflow = false;
	int significant = max_digits(x, y), initial_digit;
	for (int i = 0; i <= significant; i++) {
		initial_digit = x->digit[i];
		x->digit[i] += y->digit[i] + overflow;
		overflow = x->digit[i] < initial_digit; 
	}
	if (overflow and significant < UINT1024_SIZE - 1)
		x->digit[significant + 1] = 1;
}

void inc(uint1024_t *x) {
	bool overflow = true;
	int significant = count_significant_digits(x), initial_digit;
	for (int i = 0; i <= significant; i++) {
		initial_digit = x->digit[i];
		x->digit[i] += overflow;
		overflow = x->digit[i] < initial_digit; 
	}
	if (overflow and significant < UINT1024_SIZE - 1)
		x->digit[significant + 1] = 1;
}

uint1024_t *substract(uint1024_t *x, uint1024_t *y) {
	uint1024_t *result = init();
	bool underflow = false;
	for (int i = 0; i < UINT1024_SIZE; i++) {
		result->digit[i] = x->digit[i] - y->digit[i] - underflow;
		underflow = x->digit[i] < result->digit[i]; 
	}
	return result;
}

void lsubstract(uint1024_t *x, uint1024_t *y) {
	int initial_digit;
	bool underflow = false;
	for (int i = 0; i < UINT1024_SIZE; i++) {
		initial_digit = x->digit[i];
		x->digit[i] -= y->digit[i] + underflow;
		underflow = initial_digit < x->digit[i]; 
	}
}

void dec(uint1024_t *x) {
	int initial_digit;
	bool underflow = true;
	for (int i = 0; i < UINT1024_SIZE; i++) {
		initial_digit = x->digit[i];
		x->digit[i] -= underflow;
		underflow = initial_digit < x->digit[i]; 
	}
}

uint1024_t *mult(uint1024_t *x, uint1024_t *y) {
	uint1024_t *result = init();
	for (uint1024_t *i = uint1024_from_uint(0); compare(i, y) < 0; inc(i))
		ladd(result, x);
	return result;
}

void lmult(uint1024_t *x, uint1024_t *y) {
	uint1024_t *temp = copy(x);
	for (uint1024_t *i = uint1024_from_uint(1); compare(i, y) < 0; inc(i)) {
		ladd(x, temp);
	}
	free(temp);
}

uint1024_div divmod(uint1024_t *dividend, uint1024_t *divisor) {
	uint1024_div result;
	result.quot = init();
	uint1024_t *temp = copy(dividend);
	while (compare(temp, divisor) >= 0) {
		inc(result.quot);
		lsubstract(temp, divisor);
	}
	result.rem = temp;
	return result;
}

void ldivmod(uint1024_t *dividend, uint1024_t *divisor, uint1024_t *mod) {
	uint1024_t *quot = init();
	copyto(dividend, mod);
	while (compare(mod, divisor) >= 0) {
		inc(quot);
		lsubstract(mod, divisor);
	}
	copyto(quot, dividend);
	free(quot);
}

uint1024_t *divide(uint1024_t *dividend, uint1024_t *divisor) {
	uint1024_div res = divmod(dividend, divisor);
	free(res.rem);
	return res.quot;
}

void ldivide(uint1024_t *dividend, uint1024_t *divisor) {
	uint1024_div res = divmod(dividend, divisor);
	copyto(res.quot, dividend);
	free(res.rem);
	free(res.quot);
}

uint1024_t *mod(uint1024_t *dividend, uint1024_t *divisor) {
	uint1024_t *temp = copy(dividend);
	while (compare(temp, divisor) >= 0) {
		lsubstract(temp, divisor);
	}
	return temp;
}

void lmod(uint1024_t *dividend, uint1024_t *divisor) {
	while (compare(dividend, divisor) >= 0) {
		lsubstract(dividend, divisor);
	}
}

char *to_str(uint1024_t *x) {
	uint1024_t *current = init(), *temp = copy(x);
	uint1024_t *base = uint1024_from_uint(10);
	int length = count_significant_digits(x);
	int leading_zeros = 0;
	char *str = malloc(3 * length + 4);
	for (int i = 3 * length + 2; i >= 0; i--) {
		ldivmod(temp, base, current);
		str[i] = current->digit[0] + '0';
	}
	str[3 * length + 3] = '\0';
	while (str[leading_zeros] == '0')
		leading_zeros++;
	for (int i = leading_zeros; str[i]; i++)
		str[i - leading_zeros] = str[i];
	str[3 * length + 3 - leading_zeros] = '\0';	
	return str;
}

uint1024_t *scan_uint1024(char *str) {
	int i = 0;
	char c = str[0];
	uint1024_t *base = uint1024_from_uint(10), *temp;
	uint1024_t *result = init();
	while (c = str[i]) {
		lmult(result, base);
		temp = uint1024_from_uint(c - '0');
		ladd(result, temp);
		free(temp);
		i++;
	}
	return result;
}

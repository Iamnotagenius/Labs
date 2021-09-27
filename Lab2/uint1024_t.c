#include <stdbool.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include "uint1024_t.h"

uint1024_t init(uint32_t size) {
	uint1024_t result;
	result.digit = calloc(size, sizeof(uuint32_t8_t));
	result.size = size;
	return result;
}

uint1024_t extent(uint1024_t x, uint32_t size) {
	x.size += size;
	x.digit = realloc(x.digit, x.size);
}

uint1024_t copy(uint1024_t x) {
	uint1024_t new = init();
	for (uint32_t i = 0; i < x.size; ++i)
		new.digit[i] = x.digit[i];
	return new;
}

void copyto(uint1024_t src, uint1024_t dest) {
	if (src.size > dest.size)
		dest.digit = realloc(dest.digit, src.size)

	for (uint32_t i = 0; i < src.size; ++i)
		dest.digit[i] = src.digit[i];

	if (src.size < dest.size) {
		for (uint32_t i = src.size; i < dest.size; ++i) {
			dest.digit[i] = 0;
		}
	}
}

uint32_t count_significant_digits(const uint1024_t x) {
	uint32_t i = x.size - 1;
	while (x.digit[i] == 0 and i > 0)
		i--;
	return i;
}

uint32_t max_digits(uint1024_t x, uint1024_t y) {
	uint32_t i = x.size > y.size ? y.size : x.size;
	while (x.digit[i] == 0 and y.digit[i] == 0 and i > 0)
		i--;
	return i;
}

uint1024_t uint1024_from_uint(unsigned int x) {
	uint1024_t value = init();
	uint32_t i = 0;
	while (x) {
		value.digit[i] = x % BASE;
		x /= BASE;
		i++;
	}
	return value;
}

uint32_t compare(uint1024_t x, uint1024_t y) {
	if (x.size > y.size)
		return 1;

	if (x.size < y.size)
		return -1;

	for (uint32_t i = max_digits(x, y); i >= 0; i--) {
		if (x.digit[i] > y.digit[i])
			return 1;
		if (x.digit[i] < y.digit[i])
			return -1;
	}

	return 0;
}

uint1024_t add(uint1024_t x, uint1024_t y) {
	uint1024_t result;
	bool overflow = false;
	uuint32_t32_t significant = max_digits(x, y);
 	result = init(significant);
	
	for (uint32_t i = 0; i <= significant; i++) {
		result.digit[i] = x.digit[i] + y.digit[i] + overflow;
		overflow = result.digit[i] >= BASE;
		if (overflow) 
			result.digit[i] -= BASE 
	}
	
	if (overflow) {
		extent(result, UINT1024_MIN_SIZE);
		result.digit[significant + 1] = 1;
	}

	return result;
}

void ladd(uint1024_t x, uint1024_t y) {
	bool overflow = false;
	uint32_t significant = max_digits(x, y);

	for (uint32_t i = 0; i <= significant; ++i) {
		x.digit[i] += y->digit[i] + overflow;
		overflow = x.digit[i] >= BASE;
		if (overflow)
			x.digit[i] -= BASE; 
	}

	if (overflow) {
		extent(x, UINT1024_MIN_SIZE);
		x.digit[significant + 1] = 1;
	}
}

void inc(uint1024_t x) {
	bool overflow = true;
	uint32_t significant = count_significant_digits(x);
	
	for (uint32_t i = 0; i <= significant; i++) {
		x.digit[i] += overflow;

		overflow = x.digit[i] >= BASE; 
		if (overflow)
			x.digit[i] -= BASE;	
}
	
	if (overflow) {
		extent(x, UINT1024_MIN_SIZE);
		x.digit[significant + 1] = 1;
	}
}

uint1024_t substract(uint1024_t x, uint1024_t y) {
	uint1024_t result = init();
	bool underflow = false;
	for (uint32_t i = 0; i < x.size; i++) {
		result.digit[i] = x.digit[i] - y.digit[i] - underflow;
		underflow = result.digit[i] >= BASE;
		if (underflow)
			 result.digit[i] += BASE;
	}
	return result;
}

void lsubstract(uint1024_t x, uint1024_t y) {
	bool underflow = false;
	for (uint32_t i = 0; i < x.size; i++) {
		x.digit[i] -= y.digit[i] + underflow;
		underflow = x.digit[i] >= BASE;
		if (underflow) 
			 x.digit[i] += BASE;
	}
}

void dec(uint1024_t x) {
	uint32_t initial_digit;
	bool underflow = true;
	for (uint32_t i = 0; i < UINT1024_SIZE; i++) {
		x.digit[i] -= underflow;
		underflow = x.digit[i] >= BASE;
		if (underflow)
			x.digit[i] += BASE; 
	}
}

uint1024_t mult(uint1024_t x, uint1024_t y) {
	uint1024_t result = init();
	for (uint1024_t i = uint1024_from_uint(0); compare(i, y) < 0; inc(i))
		ladd(result, x);
	return result;
}

void lmult(uint1024_t x, uint1024_t y) {
	uint1024_t temp = copy(x);
	for (uint1024_t i = uint1024_from_uint(1); compare(i, y) < 0; inc(i)) {
		ladd(x, temp);
	}
	free(temp);
}

uint1024_div divmod(uint1024_t dividend, uint1024_t divisor) {
	uuint32_t1024_div result;
	result.quot = init();
	uint1024_t temp = copy(dividend);
	while (compare(temp, divisor) >= 0) {
		inc(result.quot);
		lsubstract(temp, divisor);
	}
	result.rem = temp;
	return result;
}

void ldivmod(uint1024_t dividend, uint1024_t divisor, uint1024_t mod) {
	uint1024_t quot = init();
	copyto(dividend, mod);
	while (compare(mod, divisor) >= 0) {
		inc(quot);
		lsubstract(mod, divisor);
	}
	copyto(quot, dividend);
	free(quot);
}

uint1024_t divide(uint1024_t dividend, uint1024_t divisor) {
	uuint32_t1024_div res = divmod(dividend, divisor);
	free(res.rem);
	return res.quot;
}

void ldivide(uint1024_t dividend, uint1024_t divisor) {
	uuint32_t1024_div res = divmod(dividend, divisor);
	copyto(res.quot, dividend);
	free(res.rem);
	free(res.quot);
}

uint1024_t mod(uint1024_t dividend, uint1024_t divisor) {
	uint1024_t temp = copy(dividend);
	while (compare(temp, divisor) >= 0) {
		lsubstract(temp, divisor);
	}
	return temp;
}

void lmod(uint1024_t dividend, uint1024_t divisor) {
	while (compare(dividend, divisor) >= 0) {
		lsubstract(dividend, divisor);
	}
}

char *to_str(uint1024_t x) {
	bool first_digit = !(x.digit[0] / 10);
	uint32_t length = count_significant_digits(x);
	char *str = malloc(2 * length + first_digit);
	
	for (uint32_t i = 0; i <= length; i += 2) 
		sprintf(str + i, "%d", x.digit[length - i]);
	
	str[2 		

	return str;
}

uint1024_t *scan_uint1024(char *str) {
	uint32_t i = 0;
	char c = str[0];
	uint1024_t *base = uint1024_from_uint(10), *temp;
	uint1024_t *result = init();
	while (c = str[i]) {
		lmult(result, base);
		temp = uuint32_t1024_from_uint(c - '0');
		ladd(result, temp);
		free(temp);
		i++;
	}
	return result;
}

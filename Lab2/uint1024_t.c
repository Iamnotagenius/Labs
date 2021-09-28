#include <stdbool.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include "uint1024_t.h"

uint1024_t init(uint32_t size) {
	uint1024_t result;
	result.digit = calloc(size, sizeof(uint8_t));
	result.size = size;
	return result;
}

uint1024_t extent(uint1024_t x, uint32_t size) {
	x.size += size;
	x.digit = realloc(x.digit, x.size);
}

uint1024_t copy(uint1024_t x) {
	uint1024_t new = init(x.size);
	for (uint32_t i = 0; i < x.size; ++i)
		new.digit[i] = x.digit[i];
	return new;
}

void copyto(uint1024_t src, uint1024_t dest) {
	if (src.size > dest.size)
		dest.digit = realloc(dest.digit, src.size);

	for (uint32_t i = 0; i < src.size; ++i)
		dest.digit[i] = src.digit[i];

	if (src.size < dest.size) {
		for (uint32_t i = src.size; i < dest.size; ++i) {
			dest.digit[i] = 0;
		}
	}
}

uint32_t count_significant_digits(const uint1024_t x) {
	int64_t i = (int64_t)x.size - 1;
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
	uint1024_t value = init(UINT1024_MIN_SIZE);
	uint32_t i = 0;
	while (x) {
		value.digit[i] = x % BASE;
		x /= BASE;
		i++;
	}
	return value;
}

int compare(uint1024_t x, uint1024_t y) {
	if (x.size > y.size)
		return 1;

	if (x.size < y.size)
		return -1;

	for (uint32_t i = max_digits(x, y); i != UINT32_MAX; i--) {
		if (x.digit[i] > y.digit[i])
			return 1;
		if (x.digit[i] < y.digit[i])
			return -1;
	}

	return 0;
}

uint1024_t add(uint1024_t x, uint1024_t y) {
	uint32_t max_size = x.size > y.size ? x.size : y.size;
	uint1024_t result = init(max_size);
	bool overflow = false;
	
	for (uint32_t i = 0; i < max_size; i++) {
		result.digit[i] = x.digit[i] + y.digit[i] + overflow;
		overflow = result.digit[i] >= BASE;
		if (overflow) 
			result.digit[i] -= BASE;
	}
	
	if (overflow) {
		extent(result, UINT1024_MIN_SIZE);
		result.digit[max_size + 1] = 1;
	}

	return result;
}

void ladd(uint1024_t x, uint1024_t y) {
	bool overflow = false;
	uint32_t max_size = x.size > y.size ? x.size : y.size;

	for (uint32_t i = 0; i < max_size; ++i) {
		x.digit[i] += y.digit[i] + overflow;
		overflow = x.digit[i] >= BASE;
		if (overflow)
			x.digit[i] -= BASE; 
	}

	if (overflow) {
		extent(x, UINT1024_MIN_SIZE);
		x.digit[max_size + 1] = 1;
	}
}

void inc(uint1024_t x) {
	bool overflow = true;
	
	for (uint32_t i = 0; i < x.size; i++) {
		x.digit[i] += overflow;

		overflow = x.digit[i] >= BASE; 
		if (overflow)
			x.digit[i] -= BASE;	
}
	
	if (overflow) {
		extent(x, UINT1024_MIN_SIZE);
		x.digit[x.size + 1] = 1;
	}
}

uint1024_t substract(uint1024_t x, uint1024_t y) {
	uint1024_t result = init(x.size);
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
	for (uint32_t i = 0; i < x.size; i++) {
		x.digit[i] -= underflow;
		underflow = x.digit[i] >= BASE;
		if (underflow)
			x.digit[i] += BASE; 
	}
}

uint1024_t mult(uint1024_t x, uint1024_t y) {
	uint1024_t result = init(x.size);

	bool is_zero = true;
	for (uint32_t i = 0; i < x.size; ++i) 
		is_zero = is_zero && !x.digit[i];

	if (is_zero)
		return init(x.size);

	for (uint1024_t i = init(y.size); compare(i, y) < 0; inc(i))
		ladd(result, x);
	return result;
}

void lmult(uint1024_t x, uint1024_t y) {
	uint1024_t temp = copy(x);

	bool zero = false;
	for (uint32_t i = 0; i < x.size; ++i) 
		zero = x.digit[i] || zero;
	if (zero)
		return;

	for (uint1024_t i = init(y.size); compare(i, y) < 0; inc(i))
		ladd(x, temp);
	
	free(temp.digit);
}

uint1024_div divmod(uint1024_t dividend, uint1024_t divisor) {
	uint1024_div result;
	result.quot = init(UINT1024_MIN_SIZE);
	uint1024_t temp = copy(dividend);
	while (compare(temp, divisor) >= 0) {
		inc(result.quot);
		lsubstract(temp, divisor);
	}
	result.rem = temp;
	return result;
}

void ldivmod(uint1024_t dividend, uint1024_t divisor, uint1024_t mod) {
	uint1024_t quot = init(UINT1024_MIN_SIZE);
	copyto(dividend, mod);
	while (compare(mod, divisor) >= 0) {
		inc(quot);
		lsubstract(mod, divisor);
	}
	copyto(quot, dividend);
	free(quot.digit);
}

uint1024_t divide(uint1024_t dividend, uint1024_t divisor) {
	uint1024_div res = divmod(dividend, divisor);
	free(res.rem.digit);
	return res.quot;
}

void ldivide(uint1024_t dividend, uint1024_t divisor) {
	uint1024_div res = divmod(dividend, divisor);
	copyto(res.quot, dividend);
	free(res.rem.digit);
	free(res.quot.digit);
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
	uint32_t length = count_significant_digits(x);
	char *str = malloc(2 * length + 1);
   	char *cursor = str + sprintf(str, "%d", x.digit[length]);
	for (int64_t i = (int64_t)(length) - 1; i >= 0; --i)
		cursor += sprintf(cursor, "%02d", x.digit[i]);
	return str;
}

uint1024_t scan_uint1024(char *str) {
	char *cursor = str;
	uint32_t i = 0;
	uint1024_t result = init(UINT1024_MIN_SIZE);
	while (*++cursor) {}
	cursor -= 2;
	while (cursor >= str) {
		if (i + 1 > result.size)
			extent(result, UINT1024_MIN_SIZE);

		sscanf(cursor, "%2d", result.digit + i);
		++i;
		cursor -= 2;
	}

	if (cursor == str - 1)
			sscanf(str, "%1d", result.digit + i);

	return result;
}

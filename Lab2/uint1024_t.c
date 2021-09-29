#include <stdbool.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include "uint1024_t.h"


uint1024_t init(uint32_t size) {
	uint1024_t result;
	/* calloc zeros allocated memory, 
	 * so this function returns a zero equivalent */
	result.digit = calloc(size, sizeof(uint8_t));
	result.size = size;
	return result;
}

uint1024_t extend(uint1024_t x, uint32_t size) {
	x.size += size;
	x.digit = realloc(x.digit, x.size);
}

void destroy(uint1024_t x) {
	free(x.digit);
	x.size = 0;
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

uint32_t index_of_last_meaning_digit(const uint1024_t x) {
	int64_t i = (int64_t)x.size - 1;
	while (x.digit[i] == 0 and i > 0)
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

	/* comparison happens from higher digits to lower */
	for (int64_t i = x.size; i >= 0; i--) {
		if (x.digit[i] > y.digit[i])
			return 1;
		if (x.digit[i] < y.digit[i])
			return -1;
	}

	return 0;
}

/* equivalent of '+' operator, dangerous function - allocates memory without freeing it */
uint1024_t add(uint1024_t x, uint1024_t y) {
	uint32_t max_size = x.size > y.size ? x.size : y.size;
	uint1024_t result = init(max_size);
	bool overflow = false;
	
	/* lower digits has lower indexes */
	for (uint32_t i = 0; i < max_size; i++) {
		 /* if a digit on previous step exceeded BASE then we add 1 to this digit */
		result.digit[i] = x.digit[i] + y.digit[i] + overflow;
		overflow = result.digit[i] >= BASE; /* 1 if true, 0 otherwise  */
		result.digit[i] %= BASE; /* use of wheel of deductions */
	}
	
	if (overflow) {
		extend(result, UINT1024_MIN_SIZE);
		result.digit[max_size + 1] = 1;
	}

	return result;
}

/* equivalent of '+=' operator, does not allocate memory, similar to previous function */
void ladd(uint1024_t x, uint1024_t y) {
	bool overflow = false;
	uint32_t max_size = x.size > y.size ? x.size : y.size;

	for (uint32_t i = 0; i < max_size; ++i) {
		x.digit[i] += y.digit[i] + overflow;
		overflow = x.digit[i] >= BASE;
		x.digit[i] %= BASE; 
	}

	if (overflow) {
		extend(x, UINT1024_MIN_SIZE);
		x.digit[max_size + 1] = 1;
	}
}

/* equivalent of '++' operator */
void inc(uint1024_t x) {
	bool overflow = true;
	
	for (uint32_t i = 0; i < x.size; i++) {
		x.digit[i] += overflow;
		overflow = x.digit[i] >= BASE; 
		x.digit[i] %= BASE;	
	}
	
	if (overflow) {
		extend(x, UINT1024_MIN_SIZE);
		x.digit[x.size + 1] = 1;
	}
}

/* equivalent of '-' operator */
uint1024_t substract(uint1024_t x, uint1024_t y) {
	uint1024_t result = init(x.size);
	bool underflow = false;

	for (uint32_t i = 0; i < x.size; ++i) {
		result.digit[i] = abs(x.digit[i] - y.digit[i] - underflow);
		underflow = x.digit[i] < y.digit[i];

		if (underflow)
			result.digit[i] = BASE - result.digit[i];
	}

	return result;
}

/* equivalent of '-=' operator */
void lsubstract(uint1024_t x, uint1024_t y) {
	bool underflow = false;
	
	for (uint32_t i = 0; i < x.size; ++i) {
		x.digit[i] -= y.digit[i] + underflow;
		underflow = x.digit[i] >= BASE;

		if (underflow)
			x.digit[i] += BASE; 
	}
}

/* equivalent of '--' operator */
void dec(uint1024_t x) {
	uint32_t initial_digit;
	bool underflow = true;
	
	for (uint32_t i = 0; i < x.size; ++i) {
		x.digit[i] -= underflow;
		underflow = x.digit[i] >= BASE;
		if (underflow) 
			x.digit[i] += BASE; 
	}
}

/* equivalent of '*' operator */
uint1024_t mult(uint1024_t x, uint1024_t y) {
	uint1024_t result = init(x.size);
	/* checking if all of x's digits are zero */
	bool is_zero = true;
	for (uint32_t i = 0; i < x.size; ++i) 
		is_zero = is_zero && !x.digit[i];
	/* if so, then return zero */
	if (is_zero)
		return init(x.size);

	/* Do I need to comment this? */
	uint1024_t i = init(y.size);
	for (i; compare(i, y) < 0; inc(i))
		ladd(result, x);
	destroy(i);
	return result;
}

/* equivalent of '*=' operator */
void lmult(uint1024_t x, uint1024_t y) {
	uint1024_t temp = copy(x);

	/* checking if all of x's digits are zero */
	bool is_zero = true;
	for (uint32_t i = 0; i < x.size; ++i) 
		is_zero = is_zero && !x.digit[i];
	/* if so, then return zero */
	if (is_zero)
		return;

	uint1024_t i = init(y.size);
	for (i; compare(i, y) < 0; inc(i))
		ladd(x, temp);

	destroy(i);
	destroy(temp);
}

/* computes quotient and remainder of divison */
uint1024_div divmod(uint1024_t dividend, uint1024_t divisor) {
	uint1024_div result;
	result.quot = init(UINT1024_MIN_SIZE);
	uint1024_t temp = copy(dividend);
	/* principle is similar to multiplication:
	 * it counts how many times dividend can be substracted from divisor
	 * so the amount of times will be quotient
	 * and something what remains in temp will be remainder */
	while (compare(temp, divisor) >= 0) {
		inc(result.quot);
		lsubstract(temp, divisor);
	}

	result.rem = temp;
	return result;
}

/* similar to previous function, but it does not allocates memory without freeing it */
void ldivmod(uint1024_t dividend, uint1024_t divisor, uint1024_t mod) {
	uint1024_t quot = init(UINT1024_MIN_SIZE);
	copyto(dividend, mod);

	while (compare(mod, divisor) >= 0) {
		inc(quot);
		lsubstract(mod, divisor);
	}
	
	copyto(quot, dividend);
	destroy(quot);
}

/* equivalent of '/' operator */
uint1024_t divide(uint1024_t dividend, uint1024_t divisor) {
	uint1024_div res = divmod(dividend, divisor);
	destroy(res.rem);
	return res.quot;
}

/* equivalent of '/=' operator */
void ldivide(uint1024_t dividend, uint1024_t divisor) {
	uint1024_div res = divmod(dividend, divisor);
	copyto(res.quot, dividend);
	destroy(res.rem);
	destroy(res.quot);
}

/* equivalent of '%' operator */
uint1024_t mod(uint1024_t dividend, uint1024_t divisor) {
	uint1024_t temp = copy(dividend);
	while (compare(temp, divisor) >= 0) {
		lsubstract(temp, divisor);
	}
	return temp;
}

/* equivalent of '%=' operator */
void lmod(uint1024_t dividend, uint1024_t divisor) {
	while (compare(dividend, divisor) >= 0) {
		lsubstract(dividend, divisor);
	}
}

/* two last functions made me choose a BASE to be equal to 100 = 10^2
 * so i can easy scan and print my long numbers */

/* makes string representaion */
char *to_str(uint1024_t x) {
	uint32_t length = index_of_last_meaning_digit(x);
	/* every meaning digit will take at least two chars in string representation */
	char *str = malloc(2 * length + 1);

	/* making use of pointer so it is easier to locate in string */
   	char *cursor = str + sprintf(str, "%d", x.digit[length]);

	/* reading a number from higher digits to lower */
	for (int64_t i = (int64_t)(length) - 1; i >= 0; --i)
		cursor += sprintf(cursor, "%02d", x.digit[i]);
	return str;
}

uint1024_t scan_uint1024(char *str) {
	char *cursor = str;
	uint32_t digits = 0;
	uint1024_t result = init(UINT1024_MIN_SIZE);
	/* reading a string from right to left */
	/* move cursor to end of a string */
	while (*++cursor) {}
	/* move cursor 2 chars left so we can read first digit */
	cursor -= 2;
	while (cursor >= str) {
		if (digits + 1 > result.size)
			extend(result, UINT1024_MIN_SIZE);

		/* reading a 2-digit decimal number to struct */
		sscanf(cursor, "%2d", result.digit + digits);
		++digits;
		cursor -= 2;
	}
	/* if cursor moves 1 char away from str that
	 * means that string has an odd amount of decimal digits and I need to read the last one
	 * BUT if cursor moves 2 chars away then string had only two decimal digits */
	if (cursor == str - 1)
			sscanf(str, "%1d", result.digit + digits);

	return result;
}

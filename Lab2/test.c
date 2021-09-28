#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "uint1024_t.h"
#include <unistd.h>

int main(int argc, char **argv) {
	/*
	uint1024_t x = scan_uint1024(argv[1]);
	char *op = argv[2];
	uint1024_t y = scan_uint1024(argv[3]);
	uint1024_t z;;
	switch(*op) {
		case '+': z = add(x, y); break;
		case '-': z = substract(x, y); break;
		case '*': z = mult(x, y); break;
		case '/': z = divide(x, y); break;
		case '%': z = mod(x, y); break;
	}
	*/
	uint1024_t great = uint1024_from_uint(0);
	while (1) {
		printf("%s\n", to_str(great));
		inc(great);
	}
	return 0;
}

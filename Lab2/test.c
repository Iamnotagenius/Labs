#include <stdio.h>
#include <stdlib.h>
#include "uint1024_t.h"

int main(int argc, char **argv) {
	unsigned int x;
	x = strtoul(argv[1], NULL, 10);
	while (x) {
		printf("%x ", x % 256);
		x /= 256;
	}
	printf("\n");
	
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "uint1024_t.h"

int main(int argc, char **argv) {
	uint1024_t x;
	x.digit[0] = 255;
	x.digit[1] = 1;
	printf("%s\n", to_str(&x));
	
	return 0;
}

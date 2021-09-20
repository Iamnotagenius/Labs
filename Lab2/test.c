#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "uint1024_t.h"

int main(int argc, char **argv) {
	uint1024_t *x = uint1024_from_uint(atoi(argv[1]));
	uint1024_t *y = uint1024_from_uint(atoi(argv[2]));
	uint1024_div *d = divmod(x, y);
	printf("%s (%s)\n", to_str(d->quot), to_str(d->rem));
	return 0;
}

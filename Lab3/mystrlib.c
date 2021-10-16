#include <stdio.h>
#include "mystrlib.h"

void skip_until(FILE *file, char c) {
	while (fgetc(file) != c) {}
}

void read_str(FILE *file, char start, char end, char *buf) {
	char *cursor = buf, c;

	skip_until(file, start);
	
	while((c = fgetc(file)) != end) {
		*cursor = c;
		cursor++;
	}
	*cursor = 0;
}


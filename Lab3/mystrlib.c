#include <stdio.h>
#include <iso646.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "mystrlib.h"

int skip_until(FILE *file, int (*predicate)(int)) {
	int skipped = 0;
	long prev_pos;
	while (!predicate(fgetc(file)) and !feof(file)) {
		skipped++;
		prev_pos = ftell(file);
	}

	if (feof(file))
		return -1;

	fseek(file, prev_pos, SEEK_SET);
	return skipped;
}

char *read_until(FILE *file, int (*predicate)(int)) {
	char *str, c, *cursor;
	fpos_t start_pos;
	fgetpos(file, &start_pos);
	int chars_read = skip_until(file, predicate);
	fsetpos(file, &start_pos);
	if (chars_read < 0)
		return NULL;
	str = malloc((chars_read + 1) * sizeof(char));
	cursor = str;

	while (!predicate(c = fgetc(file))) {
		*cursor = c;
		cursor++;
	}
	*cursor = 0;
	return str;
}

char *read_str(FILE *file) {
	skip_until(file, isgraph);
	return read_until(file, isspace);
}

int skip_until_char(FILE *file, char c) {
	int skipped = 0;
	while (fgetc(file) != c and !feof(file)) {
		skipped++;
	}
	if (feof(file))
		return -1;
	return skipped;
}

char *read_until_char(FILE *file, char end) {
	char *str, c, *cursor;
	fpos_t start_pos;
	fgetpos(file, &start_pos);
	int chars_read = skip_until_char(file, end);
	fsetpos(file, &start_pos);
	if (chars_read < 0)
		return NULL;
	str = malloc((chars_read + 1) * sizeof(char));
	cursor = str;

	while((c = fgetc(file)) != end) {
		*cursor = c;
		cursor++;
	}
	*cursor = 0;
	return str;
}

char *read_str_between(FILE *file, char start, char end) { 
	skip_until_char(file, start);
	return read_until_char(file, end);
}


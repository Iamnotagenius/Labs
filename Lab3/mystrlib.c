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

void readlines(FILE *file, void (*action)(char *)) {
	char *buf;
	while ((buf = read_until_char(file, '\n'))) {
		free(buf);
	}
	free(buf);
	
}

void readlines_c(FILE *file, void (*action)(char *), int count) {
	char *buf;
	int i = 0;
	while ((buf = read_until_char(file, '\n')) and i < count) {
		free(buf);
		i++;
	}
}

char *read_str_between(FILE *file, char start, char end) { 
	skip_until_char(file, start);
	return read_until_char(file, end);
}

int set_pos_to(char **cursor, char c) {
	int ch_read;
	while (**cursor != c and **cursor) {
		(*cursor)++;
		ch_read++;
	}
	return ch_read;
}

char *get_substr(char **cursor) {
	while (isspace(*(++(*cursor)))) {}
	int len_sub = 0;
	char *start = (*cursor) - 1, *str;
	while (isgraph(*(++(*cursor)))) { 
		len_sub++;
	}
	str = malloc((len_sub + 1) * sizeof(char));
	
	sscanf(start, "%s", str);
	return str;
}

char *clone_str_until(char **cursor, char c) {
	char *str, *start = *cursor;
	int len_sub = 0;

	while (*((*cursor)++) != c) {
		len_sub++;
	}
	str = malloc(len_sub + 1);
	for (int i = 0; i < len_sub; ++i) {
		str[i] = *start;
		start++;
	}
	str[len_sub] = '\0';
	return str;
}

char *clone_str_between(char **cursor, char start, char end) {
	set_pos_to(cursor, start);
	(*cursor)++;
	return clone_str_until(cursor, end);
}

char **split(char *str, int* counter) {
	int words_counter = 0;
	char *cursor = str;
	while (*cursor) {
		words_counter++;
		while (isspace(*(++cursor)) and *cursor) {}
		while (isgraph(*(++cursor)) and *cursor) {}
	}
	char **result = malloc(words_counter * sizeof(char *));
	cursor = str;
	for (int i = 0; i < words_counter; ++i) {
		result[i] = get_substr(&cursor);
	}
	*counter = words_counter;
	return result;
}

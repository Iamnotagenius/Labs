#include <stdio.h>
#include <iso646.h>
#include <string.h>
#include <ctype.h>

const char USAGE_MESSAGE[] = "Usage: %s [-l, --lines | -L, --not-empty-lines | -c, --bytes | -w, --words] FILE\n";

const char *OPTIONS[4][2] = {
	{"-l", "--lines"},
	{"-L", "--not-empty-lines"},
	{"-c", "--bytes"},
	{"-w", "--words"}
};

int select_option(char *arg) {
	for (int option = 0; option < 4; option++)
		if (strcmp(OPTIONS[option][0], arg) == 0 or 
						strcmp(OPTIONS[option][1], arg) == 0)
			return option;
	return -1;
}

int count_lines(FILE *file) {
	int c, lines = 0;
	while ((c = fgetc(file)) != EOF) {
		if (c == '\n')
			lines++;
	}
	return lines;
}

int count_not_empty_lines(FILE *file) {
	int c, lines = 1, prev;
	c = fgetc(file);
	if (c == EOF)
		return 0;
	while ((c = fgetc(file)) != EOF) {
		if (prev == '\n' and c != '\n')
			lines++;
		prev = c;
	}
	return lines;
}

int count_bytes(FILE *file) {
	int c, bytes = 0;
	while ((c = fgetc(file)) != EOF) {
		bytes++;
	}
	return bytes;
}

int count_words(FILE *file) {
	int words = 0;
	fscanf(file, " ");

	while (!feof(file)) {
		words++;
		fscanf(file, "%*s ");
	}
	
	return words;
}

int main(int argc, char** argv) {
	int result;
	FILE *file;
	
	if (argc < 2) {
		fprintf(stderr, USAGE_MESSAGE, argv[0]);
		return 1;
	}
	
	if (argc > 3) {
		fprintf(stderr, "Too many arguments: %d\n", argc - 1);
		fprintf(stderr, USAGE_MESSAGE, argv[0]);
		return 1;
	}

	file = fopen(argv[argc - 1], "r");

	if (!file) {
		fprintf(stderr, "Error opening file '%s'\n", argv[argc - 1], file);
		return 1;
	}

	if (argv[1][0] == '-') {
		switch(select_option(argv[1])) {
			case 0: result = count_lines(file); break;
			case 1: result = count_not_empty_lines(file); break;
			case 2: result = count_bytes(file); break;
			case 3: result = count_words(file); break;
			default:
				fprintf(stderr, "%s: invalid option '%s'\n", argv[0], argv[1]);
				fprintf(stderr, USAGE_MESSAGE, argv[0]);
				return 1;
		}
	}
	else {
		result = count_words(file);
	}
	fclose(file);
	printf("%d\n", result);
	return 0;
}

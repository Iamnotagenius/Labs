int skip_until(FILE *file, int (*predicate)(int));
char *read_until(FILE *file, int (*predicate)(int));
char *read_str(FILE *file);
int skip_until_char(FILE *file, char c);
char *read_until_char(FILE *file, char end);
char *read_str_between(FILE *file, char start, char end);

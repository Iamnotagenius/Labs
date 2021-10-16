#include <stdio.h>
#include <time.h>
#include "mystrlib.h"

#define BUFFER_SIZE 4096

struct tm parse_date(char *buf) {
	struct tm res;
	sscanf(buf, "%d/%s/%d:%d:%d:%d %s", t
	

int main(int argc, char** argv) {
	FILE *f = fopen(argv[1], "r");
	char buf1[100];
	char buf2[100];
	read_str(f, '[', ']', buf1);
	read_str(f, '"', '"', buf2);
	printf("b1 = %s\nb2 = %s\n", buf1, buf2);
}

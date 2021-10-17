#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "mystrlib.h"

#define BUFFER_SIZE 4096

typedef struct tm tm_t;

typedef struct {
	tm_t *date;
	char *remote_addr;
	char *request;
	int status;
	int bytes_send;
} data_t;
	
const char MONTHS[][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
						   "Aug", "Sep", "Oct", "Nov", "Dec"};

tm_t *parse_date(char *str) {
	tm_t *res = malloc(sizeof(tm_t));
	char month[4];
	sscanf(str, "%d/%3s/%d:%d:%d:%d %*s", &res->tm_mday, month, &res->tm_year,
				   &res->tm_hour, &res->tm_min, &res->tm_sec);
	for (int i = 0; i < 12; ++i) {
		if (strcmp(MONTHS[i], month) == 0) {
			res->tm_mon = i;
			break;
		}
	}
	res->tm_year -= 1900;
	return res;
}

data_t parse_line(FILE *log_file) {
	data_t res;
	char *buf;
	
	res.remote_addr = read_str(log_file);
	
	buf = read_str_between(log_file, '[', ']');
	res.date = parse_date(buf);
	free(buf);

	buf = read_str_between(log_file, '"', '"');
	res.request = buf;
	
	fscanf(log_file, "%d %d", &res.status, &res.bytes_send);
	
	skip_until_char(log_file, '\n');
	
	return res;
}

void print_data(data_t data) {
	printf("%s - - [%02d/%02d/%d:%02d:%02d:%02d -0400] \"%s\" %d %d\n", 
			data.remote_addr, data.date->tm_mday, data.date->tm_mon,
			data.date->tm_year + 1900, data.date->tm_hour, data.date->tm_min,
			data.date->tm_sec, data.request, data.status, data.bytes_send);
}


int main(int argc, char** argv) {
	FILE *f = fopen(argv[1], "r");
	
	data_t data = parse_line(f);
	print_data(data);

	return 0;
}

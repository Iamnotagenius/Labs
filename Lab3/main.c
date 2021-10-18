#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "mystrlib.h"
#include "stack.h"

#define BUFFER_SIZE 4096
static int lines_read = 0;

typedef struct tm tm_t;

typedef struct {
	int hour, min; 
	char sign;
} tz_t;

typedef struct {
	time_t date;
	char *remote_addr;
	char *request;
	int status;
	int bytes_send;
} data_t;
	
const char MONTHS[][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
						   "Aug", "Sep", "Oct", "Nov", "Dec"};

time_t parse_date(char *str) {
	tm_t res;
	tz_t timezone;
	char month[4];
	sscanf(str, "%d/%3s/%d:%d:%d:%d %c%02d%02d", 
					&res.tm_mday, month, &res.tm_year,
				   	&res.tm_hour, &res.tm_min, &res.tm_sec, 
				   	&timezone.sign, &timezone.hour, &timezone.min);
	
	if (timezone.sign == '-') {
		res.tm_hour -= timezone.hour;
		res.tm_min -= timezone.min;
	}
	else {
		res.tm_hour += timezone.hour;
		res.tm_min += timezone.min;
	}
	for (int i = 0; i < 12; ++i) {		
			if (strcmp(MONTHS[i], month) == 0) {
			res.tm_mon = i;
			break;
		}
	}
	res.tm_year -= 1900;
	res.tm_isdst = -1;
	time_t time = mktime(&res);
	return time;
}

data_t parse_line(char *log_line) {
	data_t res;
	int len;
	char *cursor = log_line, *tmp;
	cursor = strtok(cursor, " ");
	tmp = malloc(strlen(cursor) + 1);
	strcpy(tmp, cursor);
	res.remote_addr = tmp;
	cursor = strchr(cursor, '\0');
	cursor++;

	cursor = strtok(cursor, "]");
	res.date = parse_date(strchr(cursor, '[') + 1);
	cursor = strchr(cursor, '\0');
	cursor++;
	
	cursor = strchr(cursor, '"');
	cursor++;
	cursor = strtok(cursor, "\"");
	tmp = malloc(strlen(cursor) + 1);
	strcpy(tmp, cursor);
	res.request = tmp;

	cursor = strchr(cursor, '\0');
	cursor++;
	sscanf(cursor, "%d %d", &res.status, &res.bytes_send);

	return res;
}

void free_data(data_t *data) {
	free(data->remote_addr);
	free(data->request);
}

static inline char *time_to_str(time_t time) {
	tm_t *buf = localtime(&time);

	char *time_str = malloc(21);
	strftime(time_str, 21, "%d/%b/%Y:%T", buf);
	return time_str;
}

static inline void print_log(data_t data) {
	char *time_str = time_to_str(data.date);
	printf("%s - - [%s] \"%s\" %d %d\n", 
			data.remote_addr, time_str, data.request, data.status, data.bytes_send);
	free(time_str);
}

int main(int argc, char** argv) {
	FILE *f = fopen(argv[1], "r");
	char *buffer = malloc(4096);
	data_t parsed;
	stack *failed = create_stack(sizeof(data_t), free_data);
	struct timespec ts1, ts2;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1);
	while (!feof(f)) {
		fgets(buffer, 4096, f);
		parsed = parse_line(buffer);
		if (parsed.status / 100 == 5) {
			push(failed, &parsed);
		}
	}
 	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2);
	
	double posix_dur = 1000.0*ts2.tv_sec + 1e-6*ts2.tv_nsec
                      - (1000.0*ts1.tv_sec + 1e-6*ts1.tv_nsec);

	printf("Time elapsed: %.2f ms\n", posix_dur);
	/*** TODO make a stack field with 'copy' function ***/
	while (pop(failed, &parsed)) {
		char *time_str = time_to_str(parsed.date);
		printf("[%s] \"%s\" === %d\n", time_str, parsed.request, parsed.status);
	}
	
	return 0;
}

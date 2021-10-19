#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "mystrlib.h"
#include "stack.h"
#include "queue.h"

#define BUFFER_SIZE 4096

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
const char *ERROR_MESSAGES[] = {
	"Internal Server Error",
	"Not Implemented",
	"Bad Gateway",
	"Gateway Timeout",
	"HTTP Version Not Supported",
	"Variant Also Negotiates",
	"Insufficient Storage",
	"Loop Detected",
	"Not Extended",
	"Network Authentication Required"
};

time_t parse_date(char *str) {
	tm_t res;
	tz_t timezone;
	char month[4];
	sscanf(str, "%d/%3s/%d:%d:%d:%d %c%02d%02d", 
					&res.tm_mday, month, &res.tm_year,
				   	&res.tm_hour, &res.tm_min, &res.tm_sec, 
				   	&timezone.sign, &timezone.hour, &timezone.min);

	switch(timezone.sign) {
		case '-':
			res.tm_hour -= timezone.hour;
			res.tm_min -= timezone.min;
			break;
		case '+':	
			res.tm_hour += timezone.hour;
			res.tm_min += timezone.min;
		break;
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

	log_line = strtok(log_line, " ");
	res.remote_addr = copy_str(log_line);

	log_line = strtok(NULL, "]");
	res.date = parse_date(strchr(log_line, '[') + 1);

	log_line = strtok(NULL, "\"");
	log_line = strtok(NULL, "\"");
	res.request = copy_str(log_line);

	log_line = strtok(NULL, "\n");
	sscanf(log_line, "%d %d", &res.status, &res.bytes_send);

	return res;
}

void free_data(data_t data) {
	free(data.remote_addr);
	free(data.request);
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
	struct timespec ts1, ts2;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1);
	FILE *f = fopen(argv[1], "r");
	if (f == NULL) {
		fprintf(stderr, "Could not open file '%s'\n", argv[1]);
		return 1;
	}

	char *buffer = malloc(BUFFER_SIZE);
	int lines_read = 0, diff = atoi(argv[2]);
	data_t parsed;
	stack *failed = create_stack(sizeof(data_t));
	queue *times = create_queue(sizeof(time_t));
	struct {
		int amount;
		time_t start;
		time_t end;
	} time_window;
	while (!feof(f)) {
		fgets(buffer, BUFFER_SIZE, f);
		parsed = parse_line(buffer);

		push_q(times, &parsed.date);
		if ((int)difftime(*(time_t *)head_q(times), *(time_t *)tail_q(times)) > diff) {
			if (times->length > time_window.amount) {
				time_window.start = *(time_t *)tail_q(times);
				time_window.end = *(time_t *)times->head->previous->item;
				time_window.amount = times->length - 1;
			}
			while ((int)difftime(*(time_t *)head_q(times), *(time_t *)tail_q(times)) > diff)
				pop_q(times, NULL);	
		}

		if (parsed.status / 100 == 5) {
			push(failed, &parsed);
		}
		lines_read++;
		fscanf(f, " ");
	}
	
	free(buffer);
	
	//puts("=== SERVER ERRORS ===");
	while (pop(failed, &parsed)) {
		char *time_str = time_to_str(parsed.date);
		//printf("[%s] (%s) Error %d: %s\n", time_str, parsed.remote_addr, parsed.status, ERROR_MESSAGES[parsed.status % 100]);
		free_data(parsed);
		free(time_str);
	}
	
	printf("Most active time window\nfrom: %sto: ", 
					ctime(&time_window.start));
	fputs(ctime(&time_window.end), stdout);
	printf("(%d requests handled)\n", time_window.amount);
	
 	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2);
	
	double posix_dur = 1000.0*ts2.tv_sec + 1e-6*ts2.tv_nsec
                      - (1000.0*ts1.tv_sec + 1e-6*ts1.tv_nsec);

	printf("Time elapsed: %.2f ms\n", posix_dur);
	return 0;
}

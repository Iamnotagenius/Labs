#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <iso646.h>
#include <stdarg.h>
#include <stdbool.h>
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

const char USAGE_MESSAGE[] = "Usage: %s log.txt [-t, --time TIME_WINDOW] [-e, --error-file FILE]\n";

typedef const struct {
	char short_opt;
	char *long_opt;
} opt_t;
const int OPTION_LEN = 2;
const opt_t OPTIONS[] = {
	{ 't', "time" },
	{ 'e', "error-file" },
};

void assign_option(int opt, char *arg, void *buf[3]) {
	switch (opt) {
		case 0:
			int d = atoi(arg);
			memcpy(buf[0], &d, sizeof(int));
			break;
		case 1:	
			if (strcmp(arg, "-") == 0) {
				memcpy(buf[1], &stdout, sizeof(FILE *));
				break;
			}
			FILE *f = fopen(arg, "w");
			if (f == NULL) {
				fprintf(stderr, "Could not open file '%s'\n", arg);
				exit(2);
			}
			memcpy(buf[1], &f, sizeof(FILE *));
			break;
	}
}

int handle_short_option(char opt, char *arg, void *buf[3]) {
	for (int i = 0; i < OPTION_LEN; ++i) {
		if (opt == OPTIONS[i].short_opt) {
			assign_option(i, arg, buf);
			return 0;
		}
	}
	return opt;
}

int handle_long_option(char *opt, char *arg, void *buf[3]) {
	for (int i = 0; i < OPTION_LEN; ++i) {
		if (strcmp(OPTIONS[i].long_opt, opt) == 0) {
			assign_option(i, arg, buf);
			return 0;
		}
	}
	return 1;
}

void invalid_option(char *opt, char *prog) {
	fprintf(stderr, "Invalid option '%s'\n", opt);
	fprintf(stderr, USAGE_MESSAGE, prog);
	exit(1);
}

void parse_args(int argc, char **argv, ...) {
	if (argc < 2) {
		fprintf(stderr, USAGE_MESSAGE, argv[0]);
		exit(1);
	}
	va_list args;
	va_start(args, argv);
	void *buf[3] = {
		va_arg(args, void *),
		va_arg(args, void *),
		va_arg(args, void *)
	};
	bool reading_args = true;
	int arg = 1, opt;
	while (arg != argc) {
		if (reading_args and argv[arg][0] == '-') {
			if (argv[arg][1] == '-') {
				if (strlen(argv[arg]) == 2) {
					reading_args = false;
					arg++;
					continue;
				}
				if (handle_long_option(argv[arg] + 2, argv[arg + 1], buf)) {
					invalid_option(argv[arg], argv[0]);
				}
				arg++;
			}
			else {
				if (strlen(argv[arg]) == 1) {
					invalid_option("-", argv[0]);
				}
				
				if ((opt = handle_short_option(argv[arg][1], argv[arg + 1], buf))) {
					char *tmp = calloc(2, 1);
					*tmp = opt;
					invalid_option(tmp, argv[0]);
					free(tmp);
				}
				arg++;
			}
		}
		else {
			FILE *log_file = fopen(argv[arg], "r");
			if (log_file == NULL) {
				fprintf(stderr, "Could not open file '%s'\n", argv[arg]);
				exit(2);
			}
			memcpy(buf[2], &log_file, sizeof(FILE *));
		}
		arg++;
	}
}

char *copy_str(char *src) {
	char *dest = malloc(strlen(src) + 1);
	strcpy(dest, src);
	return dest;
}

time_t parse_date(char *str) {
	tm_t res;
	tz_t timezone;
	char month[4];
	sscanf(str, "%d/%3s/%d:%d:%d:%d %c%02d%02d", 
					&res.tm_mday, month, &res.tm_year,
				   	&res.tm_hour, &res.tm_min, &res.tm_sec, 
				   	&timezone.sign, &timezone.hour, &timezone.min);
	
	/* converting to UTC */
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
	/* in struct tm years counts since 1900 */
	res.tm_year -= 1900;
	res.tm_isdst = -1;
	time_t time = mktime(&res);
	return time;
}

data_t parse_line(char *log_line) {
	data_t res;

	/* https://en.cppreference.com/w/c/string/byte/strtok */
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

/* Using recommended way of converting time_t object to string */
char *time_to_str(time_t time) {
	tm_t *buf = localtime(&time);
	
	char *time_str = malloc(21);
	strftime(time_str, 21, "%d/%b/%Y:%T", buf);
	return time_str;
}

int main(int argc, char** argv) {
	int diff = -1;
	FILE *error_file = NULL, *log_file;
	char *buffer = malloc(BUFFER_SIZE);
	data_t parsed;
	stack *failed = create_stack(sizeof(data_t));
	queue *times = create_queue(sizeof(time_t));
	parse_args(argc, argv, &diff, &error_file, &log_file);
	if (diff == -1)
		diff = 60;

	struct {
		int amount;
		time_t start;
		time_t end;
	} time_window;

	while (!feof(log_file)) {
		fgets(buffer, BUFFER_SIZE, log_file);
		parsed = parse_line(buffer);

		if (times->length > 0 and difftime(parsed.date, *(time_t *)tail_q(times)) > diff) {
			if (times->length > time_window.amount) {
				time_window.start = *(time_t *)tail_q(times);
				time_window.end = *(time_t *)head_q(times);
				time_window.amount = times->length;
			}
			while (times->length > 0 and difftime(parsed.date, *(time_t *)tail_q(times)) > diff)
				pop_q(times, NULL);	
		}
		push_q(times, &parsed.date);

		if (parsed.status / 100 == 5) {
			push(failed, &parsed);
		}
		fscanf(log_file, " ");
	}
	
	free(buffer);
	
	printf("There were %d server errors.\n", failed->length);
	while (pop(failed, &parsed)) {
		char *time_str = time_to_str(parsed.date);
		if (error_file != NULL)
			fprintf(error_file, "[%s] (%s) Error %d: %s\n", time_str, parsed.remote_addr, parsed.status, ERROR_MESSAGES[parsed.status % 100]);
		free_data(parsed);
		free(time_str);
	}

	printf("Most active time window\nfrom: %s\nto: %s\n(%d requests)\n", 
					time_to_str(time_window.start),
					time_to_str(time_window.end),
					time_window.amount);
	
	return 0;
}

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

const char USAGE_MESSAGE[] = "Usage: %s log.txt [-t, --time TIME_WINDOW] [-e, --error-file FILE] [-f, --format ERROR_FORMAT]\n";

const char HELP[] = "NASA Log parser.\nAvailable options are:\n"
					"    -t, --time         -- Searches time period when the highest amount of requests were handled (Default: 1 minute)\n"
					"    -e, --error-file   -- Dumps requests with 5xx status to specified file (Default: don't dump to anything)\n"
					"    -f, --error-format -- Specify format for dumping errors. Available format specifiers:\n"
					"        %%d - date of request\n"
					"        %%a - remote address\n"
					"        %%r - request\n"
					"        %%s - status\n"
					"        %%b - bytes send by request\n"
					"        %%%% - literal '%'\n";


typedef const struct {
	char short_opt;
	char *long_opt;
	bool has_arg;
	void (*assign)(char *arg, void *pvar);
} opt_t;

const int MANDATORY_ARGS = 1;
const int MAX_FORMAT_LENGTH = 256;

void assign_error_file(char *arg, void *pvar) {
	if (strcmp(arg, "-") == 0) {
		*(FILE **)pvar = stdout;
		return;
	}

	FILE *f = fopen(arg, "w");
	if (f == NULL) {
		fprintf(stderr, "Could not open file '%s'\n", arg);
		exit(2);
	}
	*(FILE **)pvar = f;
}

void assign_int(char *arg, void *pvar) {
	int d = atoi(arg);
	*(int *)pvar = d;
}

void assign_str(char *arg, void *pvar) {
	*(char **)pvar = malloc(MAX_FORMAT_LENGTH);
	strncpy(*(char **)pvar, arg, MAX_FORMAT_LENGTH);
}

const int TIME_DEFAULT = 60;
const opt_t OPTIONS[] = {
	{ 't', "time", true, assign_int },
	{ 'e', "error-file", true, assign_error_file },
	{ 'f', "error-format", true, assign_str }
};

void invalid_option(char *opt, char *prog) {
	fprintf(stderr, "Invalid option '%s'\n", opt);
	fprintf(stderr, USAGE_MESSAGE, prog);
	exit(1);
}

void parse_args(int argc, char **argv, ...) {
	if (argc < 2) {
		fprintf(stderr, USAGE_MESSAGE, argv[0]);
		printf("Type '%s -h' or '%s --help' to get help.\n", argv[0], argv[0]);
		exit(1);
	}
	if (strcmp(argv[1], "-h") == 0 or strcmp(argv[1], "--help") == 0) {
		printf(HELP);
		exit(1);
	}
	bool reading_args = true, is_log_file_provided = false;
	int arg = 1, opt, opts_len = sizeof(OPTIONS)/sizeof(opt_t) + MANDATORY_ARGS,
			dashes;
	va_list va_args;
	void *args[opts_len];
	
	va_start(va_args, argv);
	for (int i = 0; i < opts_len; ++i) {
		args[i] = va_arg(va_args, void *);
	}
	va_end(va_args);
	while (arg != argc) {
		if (reading_args and argv[arg][0] == '-') {
			if (strcmp(argv[arg], "--") == 0) {
				reading_args = false;
				arg++;
				continue;
			}
			dashes = strspn(argv[arg], "-");
			for (int i = 0; i < opts_len - MANDATORY_ARGS; ++i) {
				if (strcmp(argv[arg] + dashes, OPTIONS[i].long_opt) == 0 or 
								argv[arg][dashes] == OPTIONS[i].short_opt and dashes == 1) {
					if (OPTIONS[i].has_arg) {
						if (arg + 1 >= argc) {
							fprintf(stderr, "Provide an argument to '%s' option\n", argv[arg]);
							exit(1);
						}
						OPTIONS[i].assign(argv[arg + 1], args[i + MANDATORY_ARGS]);
						arg++;
					}
					else {
						OPTIONS[i].assign(argv[arg], NULL);
					}
					goto end_while;
				}
			}
			invalid_option(argv[arg], argv[0]);
		}
		else {
			FILE *log_file = fopen(argv[arg], "r");
			if (log_file == NULL) {
				fprintf(stderr, "Could not open file '%s'\n", argv[arg]);
				exit(2);
			}
			*(FILE **)args[0] = log_file;
			is_log_file_provided = true;
		}
end_while:
		arg++;
	}
	if (!is_log_file_provided) {
		fprintf(stderr, "Log file is not provided.\n");
		fprintf(stderr, USAGE_MESSAGE, argv[0]);
		exit(1);
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

void data_to_str(data_t data, char *format, char *buf) {
	char *escsymb, *tmp;
	while (*format) {
		escsymb = strpbrk(format, "%\\");
		if (escsymb == NULL) {
			strcpy(buf, format);
			break;
		}
		strncpy(buf, format, escsymb - format);
		buf += escsymb - format;
		if (*escsymb == '%') {
			switch(*(escsymb + 1)) {
				case 'a':
					/* remote_addr */
					strcpy(buf, data.remote_addr);
					buf += strlen(data.remote_addr);
					break;
				case 'r':
					/* request */
					strcpy(buf, data.request);
					buf += strlen(data.request);
					break;
				case 'd':
					/* date */
					tmp = time_to_str(data.date);
					strcpy(buf, tmp);
					buf += strlen(tmp);
					free(tmp);
					break;
				case 'b':
					/* bytes_send */
					buf += sprintf(buf, "%d", data.bytes_send);
					break;
				case 's':
					/* status */
					buf += sprintf(buf, "%d", data.status);
					break;
				case '%':
					*buf = '%';
					buf++;
			}
		}
		else if (*escsymb == '\\') {
			switch(*(escsymb + 1)) {
				case 't':
					*buf = '\t';
					break;
				case 'n':
					*buf = '\n';
					break;
				case 'r':
					*buf = '\r';
					break;
				case '\\':
					*buf = '\\';
					break;
				
			}
			buf++;
		}
		format = escsymb + 2;
	}
}

int main(int argc, char** argv) {
	int diff = 60;
	FILE *error_file = NULL, *log_file;
	char *buffer = malloc(BUFFER_SIZE), *error_format = "Error %s: %r";
	data_t parsed;
	stack *failed = create_stack(sizeof(data_t));
	queue *times = create_queue(sizeof(time_t));
	parse_args(argc, argv, &log_file, &diff, &error_file, &error_format);

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
	
	
	printf("There were %d server errors.\n", failed->length);
	
	while (pop(failed, &parsed)) {
		char *time_str = time_to_str(parsed.date);
		if (error_file != NULL) {
			data_to_str(parsed, error_format, buffer);
			fprintf(error_file, "%s\n", buffer);
		}
		free_data(parsed);
		free(time_str);
	}

	free(buffer);

	printf("Most active time window\nfrom: %s\nto: %s\n(%d requests)\n", 
					time_to_str(time_window.start),
					time_to_str(time_window.end),
					time_window.amount);
	
	return 0;
}

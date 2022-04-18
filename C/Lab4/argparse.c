#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iso646.h>
#include <stdarg.h>
#include <stdbool.h>

/* assign functions and OPTIONS list in args.c file */

#include "args.c"

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
		exit(0);
	}
	bool reading_args = true;


	int arg = 1, swithes_count = sizeof(SWITCHES)/sizeof(switch_t),
			args_count = sizeof(POS_ARGS)/sizeof(pos_arg_t), dashes,
            current_pos_arg = 0;
	va_list va_args;
    void *args[args_count];
	void *switches[swithes_count];
	
	va_start(va_args, argv);
	for (int i = 0; i < args_count; ++i) {
		args[i] = va_arg(va_args, void *);
	}
	for (int i = 0; i < swithes_count; ++i) {
		switches[i] = va_arg(va_args, void *);
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
			for (int i = 0; i < swithes_count; ++i) {
				if (strcmp(argv[arg] + dashes, SWITCHES[i].long_opt) == 0 or 
								argv[arg][dashes] == SWITCHES[i].short_opt and dashes == 1) {
					if (SWITCHES[i].has_arg) {
						if (arg + 1 >= argc) {
							fprintf(stderr, "Provide an argument to '%s' option\n", argv[arg]);
							exit(1);
						}
						SWITCHES[i].assign(argv[arg + 1], switches[i]);
						arg++;
					}
					else {
						SWITCHES[i].assign(argv[arg], switches[i]);
					}
					goto end_while;
				}
			}
			invalid_option(argv[arg], argv[0]);
		}
		else {
            POS_ARGS[current_pos_arg].assign(argv[arg], args[current_pos_arg]);
            current_pos_arg++;
		}
end_while:
		arg++;
	}
	if (current_pos_arg < args_count) {
        
		fprintf(stderr, "%s\n", POS_ARGS[current_pos_arg].message_on_null);
		fprintf(stderr, USAGE_MESSAGE, argv[0]);
		exit(1);
    }
}

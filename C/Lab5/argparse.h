#include <stdbool.h>

typedef const struct {
	char short_opt;
	char *long_opt;
	bool has_arg;
	void (*assign)(char *arg, void *pvar);
} switch_t;

typedef struct {
    void (*assign)(char *arg, void *pvar);
    char *message_on_null;
} pos_arg_t;

void parse_args(int argc, char **argv, ...);

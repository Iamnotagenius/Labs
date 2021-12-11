#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "argparse.h"

const char USAGE_MESSAGE[] = "Usage: %s first_gen.bmp [-i, --iter MAX_ITERATIONS] [-d, --dump-freq DUMP_FREQUENCY] [-o, --output-dir DIR]\n";

#ifdef SDL
const char HELP[] = "Game of Life implementation. Available options are:\n"
                    "    -d, --dump-freq -- Specify a frequency to dump generation.\n"
                    "    -o, --output-dir -- Directory for dump files. (Default: Game_of_Life)\n"
                    "    -g, --gui        -- Open demonstration window\n";
#else
const char HELP[] = "Game of Life implementation. Available options are:\n"
                    "    -d, --dump-freq -- Specify a frequency to dump generation.\n"
                    "    -o, --output-dir -- Directory for dump files. (Default: Game_of_Life)\n";
#endif


void set_switch(char *arg, void *pvar) {
    *(bool *)pvar = true;
}

void set_str(char *arg, void *pvar) {
    *(char **)pvar = arg;
}

void set_int(char *arg, void *pvar) {
    *(unsigned *)pvar = atoi(arg);
}

const pos_arg_t POS_ARGS[] = {
    { set_str, "Error: No bmp file provided"},
};

const switch_t SWITCHES[] = {
    { 'i', "iter", true, set_int },
    { 'd', "dump-freq", true, set_int },
    { 'o', "output-dir", true, set_str },
#ifdef SDL
    { 'g', "gui", false, set_switch }
#endif
};

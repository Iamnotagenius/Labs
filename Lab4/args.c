#include <stdbool.h>
#include <string.h>
#include "argparse.h"

const char USAGE_MESSAGE[] = "Usage: %s file.mp3 [-g, --get FRAME1,FRAME2,FRAME3...] [-s, --set FRAME1=VALUE1,FRAME2=VALUE2...]\n";

const char HELP[] = "ID3V2 tag parser. Without arguments prints out all text and comment frames. \nAvailable options are:\n"
					"    -g, --get     -- Prints out only specified frames\n"
                    "    -s, --set     -- Sets values to frames\n"
                    "    -R, --rewrite -- Rewrite the whole tag";


void set_switch(char *arg, void *pvar) {
    *(bool *)pvar = true;
}

void set_str(char *arg, void *pvar) {
    *(char **)pvar = arg;
}

const pos_arg_t POS_ARGS[] = {
    { set_str, "Error: No MP3 file provided"}
};

const switch_t SWITCHES[] = {
	{ 'g', "get", true, set_str },
	{ 's', "set", true, set_str },
    { 'R', "rewrite", false, set_switch }
};
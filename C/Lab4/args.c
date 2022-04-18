#include <stdbool.h>
#include <string.h>
#include "argparse.h"

const char USAGE_MESSAGE[] = "Usage: %s file.mp3 [-x, --extract-pictures] [-g, --get FRAME1,FRAME2,FRAME3...] [-R, --rewrite] [-s, --set FRAME1=VALUE1,FRAME2=VALUE2...]\n";

const char HELP[] = "ID3V2 tag parser. Without arguments prints out all text and comment frames. \nAvailable options are:\n"
					"    -g, --get              -- Prints out only specified frames\n"
                    "    -s, --set              -- Sets values to frames\n"
                    "    -R, --rewrite          -- Rewrite the whole tag\n"
                    "    -x, --extract-pictures -- Extract APIC frames\n"
                    "The format of APIC frame is: APIC=[%%]xx:picture.png:Some description\n"
                    "Where xx is the hexadecimal picture type and if you want to embed an image, precede a %% before type\n";


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
    { 'R', "rewrite", false, set_switch },
    { 'x', "extract-pictures", false, set_switch }
};
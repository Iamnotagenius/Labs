#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argparse.h"
#include "gof.h"

int main(int argc, char **argv) {
    char *outdir = "Game_of_Life", *input_bmp;
    unsigned dumpfreq = 1, maxiter = 1024;
    parse_args(argc, argv, &input_bmp, &maxiter, &dumpfreq, &outdir);
    FILE *bmp = fopen(argv[1], "rb");
    gof_field_t *f = get_from_bmp(bmp),
				*test = malloc(sizeof(gof_field_t) + 20);	
	memcpy(test->field,"\xA0\0\0\0\x40\0\0\0\xA0\0\0\0", 12);
	test->height=3;
	test->width=3;
	test->row_size=4;
	
    fclose(bmp);
    bmp = fopen("test.bmp", "wb");
    dump_field(f, bmp);
    return 0;
}

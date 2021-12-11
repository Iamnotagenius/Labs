#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argparse.h"
#include "gof.h"

int main(int argc, char **argv) {
    char *outdir = ".", *input_bmp, *dump_filename = "%s/%04d.bmp", filename_buffer[100];
    unsigned dumpfreq = 1, maxiter = 1024, dumpcounter = 0;
    parse_args(argc, argv, &input_bmp, &maxiter, &dumpfreq, &outdir);
    FILE *bmp = fopen(input_bmp, "rb"), *dumpfile;
    gof_field_t *f = get_from_bmp(bmp);
    dump_field(f, fopen("test.bmp", "wb"));
	
    for (int iteration = 0; iteration < maxiter; ++iteration) {
        perform_iteration(f);
        if (iteration % dumpfreq == 0) {
            sprintf(filename_buffer, dump_filename, outdir, dumpcounter);
            dumpfile = fopen(filename_buffer, "wb");
            if (dumpfile == NULL) {
                fprintf(stderr, "%s: %s: No such file or directory\n", argv[0], filename_buffer);
                exit(1);
            }
            dump_field(f, dumpfile);
            fclose(dumpfile);
            dumpcounter++;
        }
    }
    return 0;
}

#include <stdio.h>
#include "arc.h"

int main(int argc, char **argv) {
    set_prog_name(argv[0]);
    append_files(argv[1], 3, argv + 2);
    list_files(argv[1]);
    char *add[] = {
        "Makefile"
    };
    append_files(argv[1], 1, add);
    list_files(argv[1]);
    extract_files(argv[1]);
}
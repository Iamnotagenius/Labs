#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "arc.h"

static char* prog_name; 
#define ERROR(msg, code, ...) fprintf(stderr, "%s: " msg "\n", prog_name, __VA_ARGS__); \
                         exit(code)

#define ARC_SIGNATURE "LOL123"
#define LEN(arr) sizeof(arr)/sizeof(arr[0])

void create_archive(char *arcname, int count, ...) {
    va_list files;
    va_start(files, count);
    FILE *arc = fopen(arcname, "wb");
    if (arc == NULL) {
        ERROR("Could not open file '%s'", 1, arcname);
    }
    fwrite(ARC_SIGNATURE, LEN(ARC_SIGNATURE), 1, arc);
    for (int i = 0; i < count; ++i) {
        char *filename = va_arg(files, char*);
        unsigned size;
        int c;
        fwrite(filename, 1, strlen(filename), arc);
        
        FILE *file = fopen(filename, "rb");
        if (file == NULL) {
            ERROR("%s: No such file or directory", 2, filename);
        }
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        rewind(file);
        fwrite(&size, sizeof size, 1, arc);
        while ((c = fgetc(file)) != EOF) {
            fputc(c, arc);
        }
    }
}


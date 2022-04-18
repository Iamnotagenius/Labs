#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "arc.h"

#ifdef __linux__
#include <sys/stat.h>
#else
#include <direct.h>
#endif

#define ERROR(msg, code, ...) fprintf(stderr, "%s: " msg "\n", prog_name, __VA_ARGS__); \
                         exit(code)

#define ARC_SIGNATURE "LOL123"
#define LEN(arr) sizeof(arr)/sizeof(arr[0])
#define MAX_FILENAME 128

static const char* prog_name; 

void set_prog_name(const char *name) {
    prog_name = name;
}

void write_file_to_arc(FILE *arc, const char *filename) {
    unsigned size;
    int c;
    fputc(0, arc);
    
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        ERROR("%s: No such file or directory", 2, filename);
    }
    
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    fwrite(filename, 1, strlen(filename), arc);
    fwrite(&size, sizeof size, 1, arc);
    while ((c = fgetc(file)) != EOF) {
        fputc(c, arc);
    }
    fclose(file);
}

int read_filename(FILE *arc, char *filename) {
    int i = 0;
    do {
        filename[i++] = fgetc(arc);
    } while (filename[i - 1] != '\0' and filename[i - 1] != EOF);
    if (filename[i - 1] == EOF) {
        return 1;
    }
    return 0;
}

FILE *open_arc(const char *arcname, const char *mode) {
    if (strstr(arcname, ".arc") != NULL)
        return fopen(arcname, mode);
        
    char extended_name[strlen(arcname) + 5];
    strcpy(extended_name, arcname);
    strcat(extended_name, ".arc");
    return fopen(extended_name, mode);
}

FILE *open_arc_for_read_and_check(const char *arcname) {
    FILE *arc = open_arc(arcname, "rb");
    if (arc == NULL) {
        ERROR("Could not open file '%s'", 1, arcname);
    }
    {
        char signature[LEN(ARC_SIGNATURE)];
        fread(signature, 1, LEN(ARC_SIGNATURE), arc);
        if (memcmp(signature, ARC_SIGNATURE, LEN(ARC_SIGNATURE)) != 0) {
            ERROR("%s: This does not look like my archive", 3, arcname);
        }
    }
    return arc;
}

void create_archive(const char *arcname, int count, char **files) {
    FILE *arc = open_arc(arcname, "wb");
    if (arc == NULL) {
        ERROR("Could not open file '%s'", 1, arcname);
    }
    fwrite(ARC_SIGNATURE, LEN(ARC_SIGNATURE), 1, arc);
    for (int i = 0; i < count; ++i) {
        write_file_to_arc(arc, files[i]);
    }
    fclose(arc);
}

void list_files(const char *arcname) {
    FILE *arc = open_arc_for_read_and_check(arcname);
    while (!feof(arc)) {
        char filename[MAX_FILENAME];
        int i = 0;
        unsigned size;
        if (read_filename(arc, filename) != 0) {
            break;
        }
        fread(&size, sizeof size, 1, arc);
        printf("%s\t\t%d\n", filename, size);
        fseek(arc, size, SEEK_CUR);
    }
    fclose(arc);
}

void extract_files(const char *arcname) {
    FILE *arc = open_arc_for_read_and_check(arcname);
    char *dirname = strdup(arcname);
    if (strstr(dirname, ".arc") != NULL) {
        *strrchr(dirname, '.') = '\0';
    }
    errno = 0;
    int dircode =
    #ifdef __linux__
        mkdir(dirname, S_IRWXU);
    #else
        _mkdir(dirname);
    #endif

    if (dircode < 0 and errno != EEXIST) {
        switch (errno) {
            case EACCES:
                ERROR("%s: Permission denied", 4, dirname);
            case ENAMETOOLONG:
                ERROR("%s: Name too long", 4, dirname);
            case ENOENT:
                ERROR("%s: No such file or directory", 4, dirname);
        }
    }

    char filename[MAX_FILENAME], *name;
    strcpy(filename, dirname);
    filename[strlen(dirname)] =
    #ifdef __linux__
        '/';
    #else
        '\\';
    #endif    
    name = filename + strlen(dirname) + 1;
    while (!feof(arc)) {
        int i = 0;
        FILE *file;
        unsigned size;
        if (read_filename(arc, name) != 0) {
            break;
        }
        fread(&size, sizeof size, 1, arc);
        file = fopen(filename, "wb");
        
        for (int i = 0; i < size; ++i) {
            fputc(fgetc(arc), file);
        }
        fclose(file);
    }
    fclose(arc);
}

void append_files(const char *arcname, int count, char **files) {
    FILE *arc = open_arc(arcname, "rb");
    if (arc == NULL) {
        create_archive(arcname, count, files);
        return;
    }
    fclose(arc);
    open_arc(arcname, "ab");

    for (int i = 0; i < count; ++i) {
        write_file_to_arc(arc, files[i]);
    }
    fclose(arc);
}

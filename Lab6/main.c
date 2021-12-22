#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iso646.h>
#include "arc.h"

#define PRINT_USAGE printf("Usage: %s command archive FILE1 [FILE2 FILE3 ... FILEN]\n", argv[0])
#define PRINTF_ERROR(msg, ...) fprintf(stderr, "%s: " msg "\n", argv[0], __VA_ARGS__)
#define PRINT_ERROR(msg) fprintf(stderr, "%s: " msg "\n", argv[0])


int main(int argc, char **argv) {
    set_prog_name(argv[0]);
    if (strcmp(argv[1], "help") == 0) {
        printf(
            "Archiver.\n"
            "Usage: %s command archive.arc FILE1 FILE2 FILE3 ... FILEN\n"
            "Available commands:\n"
            "    c, create, a, append -- Creates an archive with specified files (or if archive already exist, appends files to archive) \n"
            "    l, list              -- Lists all files in the archive\n"
            "    x, extract           -- Extracts all files into an <archive> directory\n", argv[0]);
        exit(1);
    }
    if (argc < 3) {
        PRINT_USAGE;
        printf("Type 'help' to get help.\n");
        exit(1);
    }
    char *command = argv[1] + strspn(argv[1], "-");
    if (strlen(command) == 1) {
        switch (*command) {
            case 'l':
                list_files(argv[2]);
                break;
            case 'x':
                extract_files(argv[2]);
                break;
            case 'c':
            case 'a':
                if (argc < 4) {
                    PRINT_ERROR("No files provided");
                    PRINT_USAGE;
                    exit(1);
                }
                append_files(argv[2], argc - 3, argv + 3);
                break;
            default:
                PRINTF_ERROR("Invalid command '%s'", command);
                PRINT_USAGE;
                exit(1);
        }
        return 0;
    }
    if (strcmp(command, "list") == 0) {
        list_files(argv[2]);
    }
    else if (strcmp(command, "extract") == 0) {
        extract_files(argv[2]);
    }
    else if (strcmp(command, "create") == 0 or strcmp(command, "append") == 0) {
        if (argc < 4) {
            PRINT_ERROR("No file provided\n");
            PRINT_USAGE;
            exit(1);
        }
        append_files(argv[2], argc - 3, argv + 3);
    }
    else {
        PRINTF_ERROR("Invalid command '%s'", command);
        PRINT_USAGE;
        exit(1);
    }
    return 0;
}
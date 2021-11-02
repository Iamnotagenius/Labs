#include <stdio.h>
#include <string.h>
#include "id3lib.h"

int main(int argc, char **argv) {
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("File not opened.\n");
        return 1;
    }
    struct id3tag tag;
    switch (read_whole_id3v2_tag(f, &tag)) {
    case TAG_NOT_FOUND:
        printf("ID3 tag has not been found.\n");
        return 1;
        break;
    case TAG_TOO_BIG_VERSION:
        printf("ID3 tag has too big version.\n");
        return 1;
    } 
    printf("Tag ID\tEnc\tData\n");
    char str[30], id[5];

    for (size_t i = 0; i < tag.frames_count; i++)
    {
        if (memcmp(tag.frames[i].id, "TXXX", 4) == 0)
            continue;
        if (tag.frames[i].id[0] == 'T') {
            text_frame_to_str(tag.frames[i], str);
            memcpy(id, tag.frames[i].id, 4);
            id[4] = '\0';
            printf("%4s\t%x\t%s\n", id, *(char *)(tag.frames[i].data), str);
            memset(str, 0, 30);
        }
    }
}
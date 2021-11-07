#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iso646.h>
#include <stdbool.h>
#include "id3lib.h"

int from_synchsafe32(char safe[4]) {
    return (safe[0] << 21) | (safe[1] << 14) | (safe[2] << 7) | (safe[3]);
}

int read_synchsafe32 (FILE *file) {
    char buffer[4];
    fread(buffer, sizeof(char), sizeof(buffer), file);
    return from_synchsafe32(buffer);
}

int read_id3v2_tag(FILE *audio_file, struct id3tag *result) {
    /* searching for tag id */
    {
        char id[4];
        id[3] = '\0';
        fread(id, sizeof(char), 3, audio_file);
        while (strcmp(id, ID3_IDENTIFIER) != 0 and !feof(audio_file))
        {
            id[1] = id[0];
            id[2] = id[1];
            id[2] = fgetc(audio_file);
        }
        if (feof(audio_file)) {
            return TAG_NOT_FOUND;
        }
        result->offset = ftell(audio_file) - 2;
    }

    /* ignore tag of version 2.5 and higher */
    if (fgetc(audio_file) > ID3V2_VERSION) {
        fseek(audio_file, 0, SEEK_SET);
        return TAG_TOO_BIG_VERSION;
    }
    fseek(audio_file, 1, SEEK_CUR);
    result->flags = fgetc(audio_file);

    /* reading tag flags */
    result->size = read_synchsafe32(audio_file);
    if (result->flags & EXTENDED_HEADER_BIT) {
        result->extended_header_size = read_synchsafe32(audio_file);
        fseek(audio_file, 1, SEEK_CUR);
        result->extended_flags = fgetc(audio_file);
    }
    else {
        result->extended_header_size = 0;
        result->extended_flags = 0;
    }
    
    /* skipping flags' data */

    for (unsigned char bit = 0x80; bit; bit >>= 1) {
        if (result->extended_flags & bit) {
            if (bit == RESTRICTIONS_BIT) {
                fseek(audio_file, 1, SEEK_CUR);
                result->restrictions = fgetc(audio_file);
                continue;
            }
            fseek(audio_file, fgetc(audio_file), SEEK_CUR);
        }
    }
    /* reading frames */

    int frames_size = 0, i = -1;
    char empty[4] = {0, 0, 0, 0};
    struct frame *frames = NULL;
    do
    {
        i++;
        frames_size += sizeof(struct frame);
        frames = realloc(frames, frames_size);
        fread(frames[i].id, sizeof(char), sizeof(frames[i].id), audio_file);
        frames[i].size = read_synchsafe32(audio_file);
        fread(&frames[i].flags, sizeof(char), sizeof(frames[i].flags), audio_file);
        frames[i].data = malloc(frames[i].size);
        fread(frames[i].data, sizeof(char), frames[i].size, audio_file);
    } while (memcmp(frames[i].id, empty, 4) != 0 and ftell(audio_file) - result->offset <= result->size);
    
    result->frames = frames;
    result->frames_count = i;
    return 0;
}
void text_frame_to_str(struct frame text_frame, char *buf) {
    memcpy(buf, text_frame.data + 1, text_frame.size - 1);
    buf[text_frame.size] = '\0';
}
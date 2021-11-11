#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iso646.h>
#include <stdbool.h>
#include "id3lib.h"

int from_synchsafe32(char safe[4]) {
    return (safe[0] << 21) | (safe[1] << 14) | (safe[2] << 7) | (safe[3]);
}
    
/* hashing algorithm which does not have collisions on declared frames */
int hash_frame_id(char *id) {
    return (id[0] + (id[1] << 3) + (id[2] << 6) + (id[3] << 9)) % 632;
}

bool has_frame(char *id, struct id3tag *tag) {
    return memcmp(tag->frames[hash_frame_id(id)].id, "\0\0\0\0", 4) != 0;
}

static int last_viewed = -1;
struct frame *next(struct id3tag *tag) {
    if (last_viewed == 628) {
        last_viewed = -1;
        return NULL;
    }
    while (memcmp(tag->frames[++last_viewed].id, "\0\0\0\0", 4) == 0) {
    }
    return tag->frames + last_viewed;
}


struct frame *get_frame(char *id, struct id3tag *tag) {
    return tag->frames + hash_frame_id(id);
}

void to_synchsafe32(int value, char *buf) {
    buf[3] = value & SYNCHSAFE_MASK;
    buf[2] = value >> 7 & SYNCHSAFE_MASK;
    buf[1] = value >> 14 & SYNCHSAFE_MASK;
    buf[0] = value >> 21 & SYNCHSAFE_MASK;
}

/* reads 4-byte synchsafe integer from a file */
int read_synchsafe32(FILE *file) {
    char buffer[4];
    fread(buffer, sizeof(char), sizeof(buffer), file);
    return from_synchsafe32(buffer);
}

/* writes 4-byte synchsafe integer to a file */
void write_synchsafe32(FILE *file, int value) {
    char safe[4];
    to_synchsafe32(value, safe);
    fwrite(safe, 1, 4, file);
}

/* returns tag offset counting from start of a file */
int search_id3_identifier(FILE *audio_file) {
    char id[4], ver[2];
    id[3] = '\0';
    fread(id, sizeof(char), 3, audio_file);
    while (strcmp(id, ID3_IDENTIFIER) != 0 and !feof(audio_file))
    {
        id[1] = id[0];
        id[2] = id[1];
        id[2] = fgetc(audio_file);
    }
    if (feof(audio_file)) {
        return -TAG_NOT_FOUND;
    }
    fread(ver, sizeof(char), sizeof(ver), audio_file);
    if (ver[0] > ID3V2_VERSION)
        return -TAG_TOO_BIG_VERSION;
    return ftell(audio_file) - 2;
}
    
int read_tag_header(FILE *audio_file, struct id3tag *tag) {
    
    /* searching for tag id */
    tag->offset = search_id3_identifier(audio_file);
    if (tag->offset < 0)
        return tag->offset;

    tag->flags = fgetc(audio_file);

    /* reading tag flags */
    tag->size = read_synchsafe32(audio_file);
    if (tag->flags & EXTENDED_HEADER_BIT) {
        tag->extended_header_size = read_synchsafe32(audio_file);
        fseek(audio_file, 1, SEEK_CUR);
        tag->extended_flags = fgetc(audio_file);
    }
    else {
        tag->extended_header_size = 0;
        tag->extended_flags = 0;
    }
    
    /* skipping flags' data */
    for (unsigned char bit = 0x80; bit; bit >>= 1) {
        if (tag->extended_flags & bit) {
            if (bit == RESTRICTIONS_BIT) {
                fseek(audio_file, 1, SEEK_CUR);
                tag->restrictions = fgetc(audio_file);
                continue;
            }
            fseek(audio_file, fgetc(audio_file), SEEK_CUR);
        }
    }
    return tag->offset;
}

int read_id3v2_tag(FILE *audio_file, struct id3tag *result) {

    if (read_tag_header(audio_file, result) < 0) {
        return -result->offset;
    }

    /* reading frames */
    {
        /* magic number (maximum possible index) */
        struct frame *frames = calloc(629, sizeof(struct frame));
        char id[4];
        do
        {
            int hashed;
            fread(id, sizeof(char), sizeof(id), audio_file);
            hashed = hash_frame_id(id);
            if (result->extended_flags & IS_UPDATE_BIT) {
                
                if (!has_frame(id, result)) {
                    memcpy(result->frames[hashed].id, id, 4);
                }
                
                frames[hashed].size = read_synchsafe32(audio_file);
                fread(&frames[hashed].flags, sizeof(char), sizeof(frames[hashed].flags), audio_file);
                fread(&frames[hashed].flags, sizeof(char), sizeof(frames[hashed].flags), audio_file);
                if (realloc(frames[hashed].data, frames[hashed].size) == NULL) {
                    fprintf(stderr, "Memory allocation error (at read_id3v2_tag)\n");
                    exit(1);
                }
            }
            else {

                frames[hashed].size = read_synchsafe32(audio_file);
                
                fread(&frames[hashed].flags, sizeof(char), sizeof(frames[hashed].flags), audio_file);
                
                frames[hashed].data = malloc(frames[hashed].size);
                
                fread(frames[hashed].data, sizeof(char), frames[hashed].size, audio_file);
            }
            
            /* keep reading until reaching padding or end of a tag */
        } while (memcmp(id, "\0\0\0\0", 4) != 0 and 
                ftell(audio_file) - result->offset <= result->size);
        result->frames = frames;
    }
    
    return 0;
}

void prepend_new_tag(FILE *audio_file, struct id3tag *tag) {
    rewind(audio_file);
    /* writing header */
    char header[10] = "ID3";
    header[3] = ID3V2_VERSION;
    header[4] = ID3V2_REVISION;
    header[5] = tag->flags;
    to_synchsafe32(tag->size, header + 6);
    fwrite(header, 1, 10, audio_file);
    
    /* maybe i should write extended header here... */

    /* writing tags */
    struct frame *current;
    while ((current = next(tag))) {
        char frame_header[10];
        memcpy(frame_header, current->id, 4);
        to_synchsafe32(current->size, frame_header + 4);
        memcpy(frame_header, current->flags, 2);
        fwrite(frame_header, 1, 10, audio_file);
    }
}


void write_id3v2_tag(char *filename, struct id3tag *tag) {
    FILE *audio_file = fopen(filename, "r+b");
    int offset = search_id3_identifier(audio_file);

    fseek(audio_file, 6, SEEK_CUR);
    int size = read_synchsafe32(audio_file);
    /* if there's enough space before audio data, don't do thing with temp file */
    if (size + offset >= tag->size) {
        prepend_new_tag(audio_file, tag);
        /* adding padding to the end of an old tag */
        while (ftell(audio_file) <= size + offset + 10) {
            fputc(0, audio_file);
        }
        return;
    }

    FILE *temp = tmpfile();
    fseek(audio_file, size, SEEK_CUR);
    int c;
    while ((c = fgetc(audio_file)) != EOF) {
        fputc(c, temp);
    }

    prepend_new_tag(audio_file, tag);
    while (ftell(audio_file) <= 10 + tag->size) {
        fputc(0, audio_file);
    }

    /* appending data from temp file */
    rewind(temp);
    while ((c = fgetc(temp)) != EOF) {
        fputc(c, audio_file);
    }
    fclose(temp);
    fclose(audio_file);
}

void update_id3v2_tag(char *filename, struct frame *frames, int count) {
    FILE *audio_file = fopen(filename, "r+b");
    struct id3tag present;
    if (audio_file == NULL) {
        fprintf(stderr, "Cannot open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    present.offset = search_id3_identifier(audio_file);
    if (present.offset < 0) {
        fprintf(stderr, "ID3 tag was not found in '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

}

void text_frame_to_str(struct frame text_frame, char *buf) {
    memcpy(buf, text_frame.data + 1, text_frame.size - 1);
    buf[text_frame.size] = '\0';
}
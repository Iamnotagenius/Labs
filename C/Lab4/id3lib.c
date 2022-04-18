#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iso646.h>
#include <stdbool.h>
#include "id3lib.h"

bool has_frame(char id[4], struct id3tag *tag) {
    struct frame_list *list = tag->first;
    while (memcmp(list->frame.id, id, 4) != 0 or list != NULL) {
        list = list->next;
    }

    return list == NULL;
}

struct frame *get_frame(char id[4], struct id3tag *tag) {
    struct frame_list *list = tag->first;
    while (list != NULL) {
        if (memcmp(list->frame.id, id, 4) == 0)
            break;
        list = list->next;
    }

    return list == NULL ? NULL : &list->frame;
}

void put_frame(struct frame value, struct id3tag *tag) {
    struct frame_list *new_node = malloc(sizeof(struct frame_list));
    memcpy(&new_node->frame, &value, sizeof(struct frame));
    new_node->next = tag->first;
    tag->first = new_node;
}

void remove_frame(char id[4], struct id3tag *tag) {
    struct frame_list *to_remove, **helper = &tag->first;
    while (*helper != NULL or memcmp(id, (*helper)->frame.id, 4)) {
        helper = &(*helper)->next;
    }

    if (*helper == NULL)
        return;

    to_remove = *helper;
    *helper = (*helper)->next;
    free(to_remove);
}

void free_id3v2_tag(struct id3tag *tag) {
    /* deleting frame list */
    {
        struct frame_list *tmp;
        while (tag->first != NULL) {
            tmp = tag->first;
            tag->first = tag->first->next;
            free(tmp);
        }
    }
    free(tag->extended_header);
}

int from_synchsafe32(unsigned char safe[4]) {
    return safe[0] << 21 | safe[1] << 14 | safe[2] << 7 | safe[3];
}

void to_synchsafe32(int value, char *buf) {
    buf[3] = value & SYNCHSAFE_MASK;
    buf[2] = value >> 7 & SYNCHSAFE_MASK;
    buf[1] = value >> 14 & SYNCHSAFE_MASK;
    buf[0] = value >> 21 & SYNCHSAFE_MASK;
}

/* reads 4-byte synchsafe integer from a file */
int read_int(FILE *file, struct id3tag *tag) {
    unsigned char buffer[4];
    fread(buffer, 1, 4, file);
    if (tag->version[0] == 4) {
        return from_synchsafe32(buffer);
    }
    return buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
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
    return ftell(audio_file) - 5;
}

int read_tag_header(FILE *audio_file, struct id3tag *tag) {
    tag->offset = search_id3_identifier(audio_file);
    if (tag->offset < 0)
        return tag->offset;
    fseek(audio_file, -2L, SEEK_CUR);
    fread(tag->version, 1, 2, audio_file);
    tag->flags = fgetc(audio_file);

    /* reading tag flags */
    tag->size = read_int(audio_file, tag);
    if (tag->flags & EXTENDED_HEADER_BIT) {
        tag->extended_header_size = read_int(audio_file, tag);
        tag->extended_header = tag->extended_header_size > 0 ? malloc(tag->extended_header_size) : NULL;
        fseek(audio_file, 1, SEEK_CUR);
        tag->extended_flags = fgetc(audio_file);
        fread(tag->extended_header, 1, tag->extended_header_size, audio_file);
    }
    else {
        tag->extended_header_size = 0;
        tag->extended_flags = 0;
        tag->extended_header = NULL;
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
    result->first = NULL;
    /* reading frames */
    {
        struct frame read;
        do
        {
            fread(&read.id, 1, 4, audio_file);
            if (result->extended_flags & IS_UPDATE_BIT) {
                struct frame *frame = get_frame(read.id, result);
                read.size = read_int(audio_file, result);
                fread(&read.flags, sizeof(char), sizeof(read.flags), audio_file);
                fread(&read.flags, sizeof(char), sizeof(read.flags), audio_file);
                read.data = realloc(read.data, read.size);
                if (frame != NULL) {
                    free(frame->data);
                    frame->data = read.data;
                }
            }
            else {
                read.size = read_int(audio_file, result);
                fread(&read.flags, sizeof(char), sizeof(read.flags), audio_file);
                read.data = malloc(read.size);
                fread(read.data, sizeof(char), read.size, audio_file);
            }
            if (memcmp(read.id, "\0\0\0\0", 4) != 0)
                put_frame(read, result);
            /* keep reading until reaching padding or end of a tag */
        } while (memcmp(read.id, "\0\0\0\0", 4) != 0 and
                ftell(audio_file) - result->offset <= result->size);
    }
    return 0;
}

void prepend_new_tag(FILE *audio_file, struct id3tag *tag) {
    int frames_size = 0;
    rewind(audio_file);
    /* writing header */
    fwrite("ID3", 3, 1, audio_file);
    fputc(ID3V2_VERSION, audio_file);
    fputc(ID3V2_REVISION, audio_file);
    fputc(tag->flags, audio_file);
    write_synchsafe32(audio_file, tag->size);
    /* maybe i should write extended header here... */

    /* writing tags */
    struct frame_list *list = tag->first;
    while (list != NULL) {
        fwrite(list->frame.id, 1, 4, audio_file);
        write_synchsafe32(audio_file, list->frame.size);
        fwrite(list->frame.flags, 1, 2, audio_file);
        fwrite(list->frame.data, 1, list->frame.size, audio_file);
        frames_size += 10 + list->frame.size;
        list = list->next;
    }

    for (int i = 0; i < tag->size - frames_size; ++i) {
        fputc(0, audio_file);
    }
}


void write_id3v2_tag(char *filename, struct id3tag *tag) {
    FILE *audio_file = fopen(filename, "r+b");
    FILE *temp = tmpfile();
    int offset = search_id3_identifier(audio_file), size;
    fseek(audio_file, 1, SEEK_CUR);
    size = read_int(audio_file, tag);
    fseek(audio_file, size + 10, SEEK_SET);
    /* writing data to temp file */
    prepend_new_tag(temp, tag);
    while (!feof(audio_file)) {
        fputc(fgetc(audio_file), temp);
    }
    rewind(temp);
    rewind(audio_file);

    while (!feof(temp)) {
        fputc(fgetc(temp), audio_file);
    }

    fclose(temp);
    fclose(audio_file);
}

/* WIP */
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

void put_text_frame(char id[4], char *str, struct id3tag *tag) {
    if (memcmp(id, "COMM", 4) == 0) {
        struct frame new;
        memcpy(new.id, id, 4);
        memset(new.flags, 0, 2);
        new.size = strlen(str) + 5;
        new.data = malloc(new.size);
        memset(new.data, 0, 5);
        memcpy(new.data + 5, str, strlen(str));
        put_frame(new, tag);
        return;
    }


    {
        struct frame *existing = get_frame(id, tag);
        if (existing) {
            existing->size = strlen(str) + 1;
            existing->data = realloc(existing->data, existing->size);
            ((char *)existing->data)[0] = 0;
            memcpy(existing->data + 1, str, existing->size - 1);
            return;
        }
    }
    struct frame new;
    memcpy(new.id, id, 4);
    memset(new.flags, 0, 2);
    new.size = strlen(str) + 1;
    new.data = malloc(new.size + 1);
    ((char *)new.data)[0] = 0;
    memcpy(new.data + 1, str, new.size - 1);

    put_frame(new, tag);
}

int get_png_size(char *filename, int *height, int *width) {
    FILE *picture = fopen(filename, "rb");
    char signature[8];
    fread(signature, 1, 8, picture);
    if (memcmp(signature, "\211PNG\r\n\032\n", 8) != 0) {
        fclose(picture);
        return 1;
    }
    fseek(picture, 4, SEEK_CUR);
    fread(width, 4, 1, picture);
    fread(height, 4, 1, picture);
    fclose(picture);
    return 0;
}


int put_picture_frame(char *filename, bool ref, char type, char *description, struct id3tag *tag) {
    if (type == 1) {
        int width, height;
        if (get_png_size(filename, &height, &width) != 0)
            return 1;

        if (width != 32 or height != 32) {
            return 2;
        }
    }

    char *data, *cursor;
    description = description == NULL ? "" : description;
    int size;
    if (ref) {
        size = 7 + strlen(filename) + strlen(description);
        data = malloc(size);
        cursor = data;
        *cursor = 0;
        cursor++;
        memcpy(cursor, "-->", 4);
        cursor += 4;
        *cursor = type;
        cursor++;
        strcpy(cursor, description);
        cursor += strlen(description) + 1;
        memcpy(cursor, filename, strlen(filename));

        struct frame new = {
            "APIC",
            size,
            {0, 0},
            data
        };
        put_frame(new, tag);
        return 0;
    }

    char *extension = strrchr(filename, '.') + 1,
        mime_type[11];

    strcpy(mime_type, "image/");
    if (strcmp(extension, "jpeg") == 0 or strcmp(extension, "jpg") == 0) {
        strcat(mime_type, "jpeg");
    }
    else if (strcmp(extension, "png") == 0) {
        strcat(mime_type, "png");
    }

    FILE *picture = fopen(filename, "r");
    if (picture == NULL)
        return 1;


    fseek(picture, 0L, SEEK_END);
    long filesize = ftell(picture);
    rewind(picture);

    size = 7 + strlen(mime_type) + strlen(description) + filesize;

    data = malloc(size);
    cursor = data;
    *cursor = 0;
    cursor++;
    strcpy(cursor, mime_type);
    cursor += strlen(mime_type) + 1;
    *cursor = type;
    cursor++;
    strcpy(cursor, description);
    cursor += strlen(description) + 1;
    while (!feof(picture)) {
        *cursor = fgetc(picture);
        cursor++;
    }

    struct frame new = {
        "APIC",
        size,
        {0, 0},
        data
    };
    put_frame(new, tag);
    return 0;
}

void text_frame_to_str(struct frame *text_frame, char *buf) {
    if (text_frame->id[0] == 'T') {
        memcpy(buf, text_frame->data + 1, text_frame->size - 1);
        buf[text_frame->size] = '\0';
        return;
    }
    else if (memcmp(text_frame->id, "COMM", 4) == 0) {
        strcpy(buf, text_frame->data + 4);
        strcat(buf, strlen(text_frame->data + 4) ? ": " : "");
        strncat(buf, text_frame->data + strlen(text_frame->data + 4) + 5, 
            text_frame->size - strlen(text_frame->data + 4) - 5);
    }
    else if (memcmp(text_frame->id, "APIC", 4) == 0) {
        char *cur = text_frame->data + strlen(text_frame->data + 1) + 3;
        strcpy(buf, cur);
        strcat(buf, "[");
        strcat(buf, text_frame->data + 1);
        strcat(buf, "]");
    }
}

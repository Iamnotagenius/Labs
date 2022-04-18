#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iso646.h>
#include <stdint.h>

#include "gof.h"

#define ERROR(fmt) fprintf(stderr, fmt "\n");
#define PRINT_DEBUG(fmt, ...) fprintf(stderr, "%s[%d]: " fmt "\n", __FILE__, __LINE__, __VA_ARGS__);

gof_field_t *get_from_bmp(FILE *bmp) {
    char main_header[14], *full_header;
    fread(main_header, 1, 14, bmp);
    {
        if (memcmp(main_header, "BM", 2) != 0) {
            fprintf(stderr, "Not a bmp file.\n");
            exit(1);
        }
    }
    int pixel_array_offset, header_size, width, height;
    memcpy(&pixel_array_offset, main_header + 10, 4);


    fread(&header_size, 4, 1, bmp);

    full_header = malloc(header_size + 14);
    memcpy(full_header, main_header, 14);
    memcpy(full_header + 14, &header_size, 4);
    fread(full_header + 18, 1, header_size - 4, bmp);

    memcpy(&width, full_header + 18, header_size < 40 ? 2 : 4);
    if (header_size < 40) {
        memcpy(&height, full_header + 20, 2);
    }
    else {
        memcpy(&height, full_header + 22, 4);
    }
#ifdef DEBUG
    PRINT_DEBUG("pixel_array_offset = %X, %dx%d", pixel_array_offset, width, height)
#endif

    gof_field_t *new_field = malloc(sizeof(gof_field_t) + (width + 2) * (height + 2));
    new_field->width = width;
    new_field->height = height;
    new_field->row_size = width + 2;
    memset(new_field->field, 0, new_field->row_size * (height + 2));

    short bpp;
    if (header_size >= 40) {
        fseek(bmp, 28, SEEK_SET);
    }
    fread(&bpp, sizeof(short), 1, bmp);
    fseek(bmp, pixel_array_offset, SEEK_SET);

    unsigned pixel_mask = (1 << bpp) - 1, bmp_row_size = (bpp * width + 31) / 32 * 4;
    unsigned char bmp_row[bmp_row_size];

    for (int row = height * new_field->row_size; row >= new_field->row_size; row -= new_field->row_size) {
        pixel_mask = 1 << (8 - bpp);
        fread(bmp_row, 1, bmp_row_size, bmp);
        for (int pixel = 0; pixel < width; ++pixel) {
            int bits = bpp < 8 ? pixel_mask & bmp_row[(bpp * pixel) / 8]
                               : memcmp(&bmp_row[(bpp * pixel) / 8], "\xff\xff\xff\xff", bpp / 8) == 0;
            new_field->field[row + pixel + 1] = bits != 0;
            pixel_mask >>= bpp;
            if (!pixel_mask)
                pixel_mask = 1 << (8 - bpp);
        }
        //printf("field[%d] = ", row);
        //for (int pixel = 0; pixel < new_field->row_size; ++pixel) {
        //    printf("%02X ", new_field->field[row + pixel]);
        //}
        //printf("\n");
    }

    free(full_header);
    return new_field;
}


void dump_field(gof_field_t *field, FILE *bmp) {
    char header[62];
    memset(header, 0, 54);
    memcpy(header, "BM", 2);
    int filesize = 14 + 40 + 8 + field->height * field->row_size;
    memcpy(header + 2, &filesize, 4);
    /* pix array offset is always 62 */
    header[0xA] = 0x3E;
    /* using Windows BITMAPINFOHEADER */
    header[0xE] = 0x28;
    memcpy(header + 0x12, &field->width, 4);
    memcpy(header + 0x16, &field->height, 4);
    header[0x1A] = 1;
    header[0x1C] = 1;
    /* color table */
    memcpy(header + 0x36, "\0\0\0\0\xff\xff\xff\0", 8);
    fwrite(header, 1, 62, bmp);

    unsigned int row_size = (field->width + 31) / 32 * 4;
    unsigned char pixel_row[row_size];
    for (int row = (field->height - 1) * field->row_size; row >= field->row_size; row -= field->row_size) {
        memset(pixel_row, 0, row_size);
        for (int pixel = 1; pixel < field->row_size - 1; ++pixel) {
            int bits = field->field[row + pixel];
            pixel_row[pixel / 8] |= bits << (7 - (pixel % 8)); 
        }
        fwrite(pixel_row, 1, row_size, bmp);
    }
}

void perform_iteration(gof_field_t *field) {
    static gof_field_t *counter_field = NULL;

    if (counter_field == NULL or counter_field->height != field->height or counter_field->width != field->width) {
        counter_field = realloc(counter_field, sizeof(gof_field_t) + (field->width + 2) * (field->height + 2));
        counter_field->height = field->height;
        counter_field->width = field->width;
    }

    for (int i = field->row_size + 1; i < field->row_size * (field->height - 1) - 1; i += 8) {
        uint64_t *neighbours = (uint64_t *)(counter_field->field + i);
        /* top left */
        *neighbours += *(uint64_t*)(field->field + i - field->row_size - 1);
        /* top */
        *neighbours += *(uint64_t*)(field->field + i - field->row_size);
        /* top right */
        *neighbours += *(uint64_t*)(field->field + i - field->row_size + 1);
        /* left */
        *neighbours += *(uint64_t*)(field->field + i - 1);
        /* right */
        *neighbours += *(uint64_t*)(field->field + i + 1);
        /* bottom left */
        *neighbours += *(uint64_t*)(field->field + i + field->row_size - 1);
        /* bottom */
        *neighbours += *(uint64_t*)(field->field + i + field->row_size);
        /* bottom right */
        *neighbours += *(uint64_t*)(field->field + i + field->row_size + 1);
    }
    for (int i = field->row_size; i < field->row_size * (field->height - 1); i += 8) {
        uint64_t neighbours = *(uint64_t *)(counter_field->field + i) & 0x0707070707070707;
        uint64_t alive = *(uint64_t *)(field->field + i);
        uint64_t combined_state = (alive << 3) | neighbours;

        /* not B and C and (A or D) */
        combined_state = ((combined_state ^ 0x0404040404040404) >> 2) & /* not B */
                         (combined_state >> 1) & /* C */
                         ((combined_state >> 3) | combined_state); /* A or D */

        *(uint64_t *)(field->field + i) = combined_state;
    }
    for (int i = field->row_size; i < field->row_size * field->height; i += field->row_size) {
        field->field[i] = 0;
        field->field[i + field->row_size - 1] = 0;
    }
    memset(counter_field->field, 0, (field->width + 2) * (field->height + 2));
}


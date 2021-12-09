#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    gof_field_t *new_field = malloc(sizeof(gof_field_t) + (width + 31)/32 * 4 * abs(height));
    new_field->width = width;
    new_field->height = height;
    new_field->row_size = (width + 31)/32 * 4;

    short bpp;
    if (header_size >= 40) {
        fseek(bmp, 28, SEEK_SET);
    }
    fread(&bpp, sizeof(short), 1, bmp);
    fseek(bmp, pixel_array_offset, SEEK_SET);

    if (bpp > 1) {
        /* TODO: convert >1-bit bitmaps into 1-bit */
        int input_row_size = (bpp * width + 31)/32 * 4;
        char row_buf[input_row_size];
        unsigned field_buf[new_field->row_size / 4];
        for (int row = (height - 1) * new_field->row_size; row >= 0; row -= new_field->row_size) {
            /* packing row in 1-bit format */
            memset(field_buf, 0, new_field->row_size);
            fread(row_buf, 1, input_row_size, bmp);
            int byte = 0;
            for (int i = 0; i < width; ++i) {
                unsigned bit = memcmp(row_buf + byte, "\xFF\xFF\xFF\xFF", (bpp + 7) / 8) == 0;
                bit <<= (-i - 1) & 31;
                field_buf[i / 32] |= bit;
                byte += (bpp + 7) / 8;
            }
            memcpy(new_field->field + row, field_buf, new_field->row_size);
        }
    }
    else {
        for (int row = (height - 1) * new_field->row_size; row >= 0; row -= new_field->row_size) {
            fread(new_field->field + row, 1, new_field->row_size, bmp);
        }
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

    for (int row = (field->height - 1) * field->row_size; row >= 0; row -= field->row_size) {
        fwrite(field->field + row, 1, field->row_size, bmp);
    }
}


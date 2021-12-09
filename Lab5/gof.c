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

    gof_field_t *new_field = malloc(sizeof(gof_field_t) + (width + 1)/2 * (height + 2));
    new_field->width = width;
    new_field->height = height;
    new_field->row_size = (width + 1)/2;
    memset(new_field->field, 0, new_field->row_size * (height + 2));

    short bpp;
    if (header_size >= 40) {
        fseek(bmp, 28, SEEK_SET);
    }
    fread(&bpp, sizeof(short), 1, bmp);
    fseek(bmp, pixel_array_offset, SEEK_SET);

    unsigned pixel_mask = (1 << bpp) - 1, bmp_row_size = (bpp * width + 31) / 32 * 4;
    unsigned char bmp_row[bmp_row_size];
    char cell_mask = 0x10;

    for (int row = height * new_field->row_size; row >= new_field->row_size; row -= new_field->row_size) {
        pixel_mask = 1 << (8 - bpp);
        fread(bmp_row, 1, bmp_row_size, bmp);
        for (int pixel = 0; pixel < width; ++pixel) {
            int bits = bpp < 8 ? pixel_mask & bmp_row[(bpp * pixel) / 8]
                               : memcmp(&bmp_row[(bpp * pixel) / 8], "\xff\xff\xff\xff", bpp / 8) == 0;
            new_field->field[row + pixel / 2] |= cell_mask & (bits ? 0xFF : 0);
            cell_mask ^= 0x11; /* changing "half" of byte */
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
    for (int row = field->height * field->row_size; row >= field->row_size; row -= field->row_size) {
        memset(pixel_row, 0, row_size);
        for (int pixel = 0; pixel < field->row_size; ++pixel) {
            int bits = field->field[row + pixel] % 14;
            pixel_row[pixel / 4] |= bits << ((3 - (pixel % 4)) << 1); 
        }
        fwrite(pixel_row, 1, row_size, bmp);
    }
}


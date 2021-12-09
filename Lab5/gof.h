#include <stdio.h>

typedef struct {
    int width;
    int height;
    int row_size;
    char *bmp_header;
    char *bmp_footer;
    unsigned char field[];
} gof_field_t;

gof_field_t *get_from_bmp(FILE *bmp);
void dump_field(gof_field_t *field, FILE *bmp);
void perform_iteration(gof_field_t *field);

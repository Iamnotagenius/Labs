#include <stdio.h>
#include <time.h>
#define ID3_IDENTIFIER "ID3"
#define ID3_BACK_IDENTIFIER "3DI"
#define ID3V2_VERSION 04
#define ID3V2_REVISION 00

#define MINIMUM_PADDING 1024

#define UNSYNCHRONISATION_BIT   0b10000000
#define EXTENDED_HEADER_BIT     0b01000000
#define EXPERIMENTAL_BIT        0b00100000
#define FOOTER_BIT              0b00010000

#define IS_UPDATE_BIT    0b01000000
#define CRC_BIT          0b00100000
#define RESTRICTIONS_BIT 0b00010000

#define SIZE_BITS           0b11000000
#define TEXT_ENCODING_BIT   0b00100000
#define TEXT_FIELD_BITS     0b00011000
#define IMAGE_ENCODING_BIT  0b00000100
#define IMAGE_SIZE_BITS     0b00000011


#define ALTERED_BIT         0b01000000
#define FILE_ALTERED_BIT    0b00100000
#define READONLY_BIT        0b00010000

#define GROUP_BIT 0b01000000

#define COMPRESSION_BIT             0b00001000
#define ENCRYPTION_BIT              0b00000100
#define UNSYCHRONISATION_BIT        0b00000010
#define DATA_LENGTH_INDICATOR_BIT   0b00000001

#define TAG_NOT_FOUND 1
#define TAG_TOO_BIG_VERSION 2
#define UPDATE_TAG 3

#define SYNCHSAFE_MASK 0x7f

struct frame {
    char id[4];
    unsigned int size;
    char flags[2];
    void *data;
};

struct id3tag {
    char flags;
    int offset;
    unsigned int size;
    unsigned extended_header_size;
    char extended_flags;
    char *extended_header;
    char restrictions;
    struct frame_list *first;
};

struct frame_list {
    struct frame frame;
    struct frame_list *next;
};

int read_id3v2_tag(FILE *audio_file, struct id3tag *buf);
void write_id3v2_tag(char *audio_file, struct id3tag *tag);
void free_id3v2_tag(struct id3tag *tag);
struct frame *get_frame(char *id, struct id3tag *tag);
void put_frame(struct frame value, struct id3tag *tag);
void append_frame(FILE *audio_file, struct frame);
void text_frame_to_str(struct frame *text_frame, char *buf);
struct frame *next(struct id3tag *tag);
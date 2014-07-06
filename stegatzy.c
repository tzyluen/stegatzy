#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>              /* refer to /usr/include/asm-generic/errno.h */
#include "tzybitmap.h"
#include "stegatzylib.h"

#define CMD                    1
#define CMD_BITMAP_FILE        2
#define CMD_ENCODE             "encode"
#define CMD_DECODE             "decode"
#define CMD_CREATE             "create"
#define CMD_ENC_TYPE_PAD       "pad"
#define CMD_ENC_TYPE_LSB       "lsb"
#define CMD_OPT                3
#define CMD_ENC_TYPE           4
#define CMD_BITMAP_DEPTH       4
#define CMD_BITMAP_WIDTH       5
#define CMD_BITMAP_HEIGHT      6
#define CMD_BITMAP_R           7
#define CMD_BITMAP_G           8
#define CMD_BITMAP_B           9
#define CMD_SECRET_TEXT        5


void stegatzy_create_bitmap(const char *, int, int, int, char, char, char);
size_t stegatzy_encode(FILE *, const char *, char);
void stegatzy_decode();
void usage();


int main(int argc, char **argv)
{
    if (argc < 4)
        usage();

    if (strcmp(argv[CMD], CMD_ENCODE) == 0) {

        FILE *bmpfp = fopen(argv[CMD_BITMAP_FILE], "r+");

        if (strcmp(argv[CMD_ENC_TYPE], CMD_ENC_TYPE_PAD) == 0) {
            stegatzy_encode(bmpfp, argv[CMD_SECRET_TEXT], ENC_TYPE_PAD);
        } else if (strcmp(argv[CMD_ENC_TYPE], CMD_ENC_TYPE_LSB) == 0) {
            stegatzy_encode(bmpfp, argv[CMD_SECRET_TEXT], ENC_TYPE_LSB);
        }

    } else if (strcmp(argv[CMD], CMD_DECODE) == 0) {

        //FILE *bmpfp = fopen(argv[CMD_BITMAP_FILE], "rb");

        if (strcmp(argv[CMD_ENC_TYPE], CMD_ENC_TYPE_PAD) == 0) {
            printf("DECODE PAD\n");
        } else if (strcmp(argv[CMD_ENC_TYPE], CMD_ENC_TYPE_LSB) == 0) {
            printf("DECODE LSB\n");
        }

    } else if (strcmp(argv[CMD], CMD_CREATE) == 0) {

        if (strcmp(argv[CMD_OPT], "-c") == 0) {
            stegatzy_create_bitmap(  argv[CMD_BITMAP_FILE],
                                     strtol(argv[CMD_BITMAP_WIDTH], NULL, 10),
                                     strtol(argv[CMD_BITMAP_HEIGHT], NULL, 10),
                                     strtol(argv[CMD_BITMAP_DEPTH], NULL, 10),
                                     strtol(argv[CMD_BITMAP_R], NULL, 10),
                                     strtol(argv[CMD_BITMAP_G], NULL, 10),
                                     strtol(argv[CMD_BITMAP_B], NULL, 10)  );
        } else {
            ERROR_PRINT_ERR
            usage();
        }
    } else {
        usage();
    }

    return EXIT_SUCCESS;
}


void stegatzy_create_bitmap(
        const char *fname,
        int width,
        int height,
        int depth,
        char r, char g, char b)
{
    t_pixel rgb = { .red = r, .green = g, .blue = b };
    printf("Creating bmp:\n name: %s, width: %d, height: %d, depth: %d, r: %d, g: %d, g: %d\n",
            fname, width, height, depth, r, g, b);
    t_bitmap *bmp = create_bitmap(fname, width, height, depth, rgb);
    write_bitmap_file(fname, bmp);
    free(bmp);
}


size_t stegatzy_encode(FILE *fp, const char *s, char enc_type)
{
    printf("Encoding bmp:\n secret text: %s, enc_type: %d\n", s, enc_type);
    switch (enc_type) {
        case ENC_TYPE_PAD:
            stegatzy_by_padding(fp, s);
            break;
        case ENC_TYPE_LSB:
            stegatzy_by_lsb(fp, s);
            break;
        default:
            break;
    }

    return 0;
}


void stegatzy_decode()
{
}


void usage()
{
    printf("usage:\n  stegatzy <command> args\n");
    printf("Example,\n");
    printf("  stegatzy encode <bitmap file> -t [pad|lsb] \"secret message\"\n");
    printf("  stegatzy decode <bitmap file> -t [pad|lsb]\n");
    printf("  stegatzy create <bitmap file> -c <depth:[8|16|24|32]> <W> <H> <R> <G> <B>\n");

    exit(1);
}

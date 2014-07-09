#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>              /* refer to /usr/include/asm-generic/errno.h */
#include "tzybitmap.h"
#include "stegatzylib.h"

/* program's **argv index */
#define CMD                         1
#define CMD_COVER_FILE              2
#define CMD_COVER_FILE_OPT          3
#define CMD_BITMAP_CREATE           3
#define CMD_COVER_FILE_TYPE         4
#define CMD_ENC_OPT                 5
#define CMD_ENC_TYPE                6
#define CMD_BITMAP_DEPTH            4
#define CMD_BITMAP_WIDTH            5
#define CMD_BITMAP_HEIGHT           6
#define CMD_BITMAP_R                7
#define CMD_BITMAP_G                8
#define CMD_BITMAP_B                9
#define CMD_SECRET_TEXT             7

/* program's parameters constants */
#define CMD_ENCODE                  "encode"
#define CMD_DECODE                  "decode"
#define CMD_CREATE                  "create"
#define CMD_ENC_TYPE_PAD            "pad"
#define CMD_ENC_TYPE_LSB            "lsb"
#define BMP                         "bmp"
#define WAV                         "wav"


void stegatzy_create_bitmap(const char *, int, int, int, char, char, char);
size_t stegatzy_encode(FILE *, const char *, char, char, char *);
void stegatzy_decode(FILE *, char, char);
char *get_new_file_name(char *);
void usage();


int main(int argc, char **argv)
{
    if (argc < 6)
        usage();

    int cover_type, enc_type;
    char *ofn;
    if ( (strcmp(argv[CMD], CMD_ENCODE) == 0) || (strcmp(argv[CMD], CMD_DECODE) == 0) ) {

        FILE *bmpfp = fopen(argv[CMD_COVER_FILE], "r+");
        if (! bmpfp) {
            ERROR_PRINT_ERR
            return errno;
        }

        ofn = get_new_file_name(argv[CMD_COVER_FILE]);

        if (strcmp(argv[CMD_COVER_FILE_OPT], "-t") == 0) {
            cover_type = (strcmp(argv[CMD_COVER_FILE_TYPE], BMP) == 0) ? COVER_TYPE_BMP : COVER_TYPE_WAV;
            enc_type = (strcmp(argv[CMD_ENC_TYPE], "pad") == 0) ? ENC_TYPE_PAD : ENC_TYPE_LSB;

            if ( (strcmp(argv[CMD_COVER_FILE_TYPE], WAV) == 0) && (enc_type == ENC_TYPE_PAD) ) {
                printf(" WAV - doesn't supports -e pad\n");
                usage();
            }

            if (strcmp(argv[CMD], CMD_ENCODE) == 0)
                stegatzy_encode(bmpfp, argv[CMD_SECRET_TEXT], cover_type, enc_type, ofn);
            else
                stegatzy_decode(bmpfp, cover_type, enc_type);

        } else {
            printf(" Invalid opt: %s\n", argv[CMD_COVER_FILE_OPT]);
            usage();
        }

        fclose(bmpfp);

    } else if (strcmp(argv[CMD], CMD_CREATE) == 0) {
	    // stegatzy create $(BITMAPFILE) -c 24 401 401 255 255 255
        if (strcmp(argv[CMD_BITMAP_CREATE], "-c") == 0) {
            stegatzy_create_bitmap(  argv[CMD_COVER_FILE],
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


size_t stegatzy_encode(FILE *fp, const char *s, char cover_type, char enc_type, char *ofn)
{
    size_t encoded_size = 0;

    switch (enc_type) {
        case ENC_TYPE_PAD:
            if (cover_type == COVER_TYPE_BMP)
                encoded_size = stegatzy_bmp_by_padding(fp, s);
            break;
        case ENC_TYPE_LSB:
            if (cover_type == COVER_TYPE_BMP)
                encoded_size = stegatzy_bmp_by_lsb(fp, s);
            else
                encoded_size = stegatzy_wav_by_lsb(fp, s, ofn);
            break;
        default:
            break;
    }
    printf(" encoded_size: %zu bytes\n", encoded_size);

    return encoded_size;
}


void stegatzy_decode(FILE *fp, char cover_type, char enc_type)
{
    printf("Decoding bmp:\n enc_type: %d\n", enc_type);
    switch (enc_type) {
        case ENC_TYPE_PAD:
            stegatzy_bmp_decode_padding(fp);
            break;
        case ENC_TYPE_LSB:
            stegatzy_bmp_decode_lsb(fp);
            break;
        default:
            break;
    }
}


char *get_new_file_name(char *old)
{
    char *prefix = "new_";
    char *new = malloc(strlen(old) + strlen(prefix) + 1);
    strcpy(new, prefix);
    strcat(new, old);

    return new;
}


void usage()
{
    printf("usage:\n  stegatzy <command> args\n");
    printf("Example,\n");
    printf("  stegatzy encode [bitmap|wav file] -t [bmp|wav] -e [pad|lsb] \"secret message\"\n");
    printf("  stegatzy decode [bitmap|wav file] -t [bmp|wav] -e [pad|lsb]\n");
    printf("  stegatzy create [bitmap file] -c <depth:[8|16|24|32]> <W> <H> <R> <G> <B>\n");

    exit(1);
}

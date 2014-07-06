#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "tzybitmap.h"
#include "stegatzylib.h"

size_t stegatzy_by_padding(FILE *fp, const char *s)
{
    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    read_bitmap_file(fp, bmp);
    size_t padding_size = padding_check(bmp);
    size_t offset = get_rowsize(bmp->info_header.width) - padding_size;
    size_t available_encode_size = get_encode_size(bmp, ENC_TYPE_PAD);

    fseek(fp, bmp->header.pixel_offset, SEEK_SET);

    char *bp = malloc(sizeof(char) * (padding_size+1));
    size_t encoded_size = 0;
    while (strlen(s) > 0) {      /* while `secret' not fully encoded */
        if (encoded_size >= available_encode_size)
            return encoded_size;    /* exit if exceeded available space */
        if (!fseek(fp, offset, SEEK_CUR)) {
            strncpy(bp, s, padding_size);
            s += padding_size;
            fwrite(bp, padding_size, 1, fp);
            encoded_size += padding_size;
        }
    }

    return encoded_size;
}


size_t stegatzy_decode_padding(FILE *fp)
{
    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    read_bitmap_file(fp, bmp);
    size_t padding_size = padding_check(bmp);
    size_t offset = get_rowsize(bmp->info_header.width) - padding_size;
    size_t available_encode_size = get_encode_size(bmp, ENC_TYPE_PAD);
    size_t decoded_size = 0;

    fseek(fp, bmp->header.pixel_offset, SEEK_SET);

    char *secret = malloc(sizeof(char) * (available_encode_size+1));
    char *bp = secret;
    for (; decoded_size < available_encode_size && (!fseek(fp, offset, SEEK_CUR)); ) {
        fread(secret, padding_size, 1, fp);
        secret += padding_size;
        decoded_size += padding_size;
    }
    secret = bp;
    printf("%s\n", secret);

    return decoded_size;
}


size_t stegatzy_by_lsb(FILE *fp, const char *s)
{
    printf("stegatzy_by_lsb\n");
    return 0;
}


size_t get_encode_size(t_bitmap *bmp, char enc_type)
{
    switch (enc_type) {
        case ENC_TYPE_PAD:
            return padding_check(bmp) * bmp->info_header.height;
            break;
        case ENC_TYPE_LSB:
            return 0;
            break;
        default:
            return 0;
    }
}


void stream_padding_contents(FILE *fp, t_bitmap *bmp, size_t padding_size, size_t encode_size)
{
    char pad[padding_size];
    size_t offset = get_rowsize(bmp->info_header.width) - padding_size;
    size_t z,i = 0;
    fseek(fp, bmp->header.pixel_offset, SEEK_SET);

    for (; i < encode_size && !fseek(fp, offset, SEEK_CUR); ++i) {
        fread(pad, padding_size, 1, fp);
        for (z = 0; z < sizeof(pad); ++z)
            printf("pad[%zu]->%x, %c\n", z, pad[z], pad[z]);
    }
}

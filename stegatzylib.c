#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <malloc.h>
#include "tzybitmap.h"
#include "stegatzylib.h"

size_t stegatzy_by_padding(FILE *fp, const char *s)
{
    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    read_bitmap_file(fp, bmp);
    size_t padding_size = padding_check(bmp);
    size_t offset = get_rowsize(bmp->info_header.width) - padding_size;
    size_t available_encode_size = get_encode_size(bmp, ENC_TYPE_PAD);

    printf(" available_encode_size: %zu\n", available_encode_size);

    /* set fp offset point to start of pixel offset */
    fseek(fp, bmp->header.pixel_offset, SEEK_SET);

    char *bp = malloc(sizeof(char) * (padding_size+1));
    size_t encoded_size = 0;
    while (strlen(s) > 0) {        /* while `secret' not fully encoded */
        if (encoded_size >= available_encode_size)
            return encoded_size;   /* exit if exceeded available space */
        if (!fseek(fp, offset, SEEK_CUR)) {
            strncpy(bp, s, padding_size);
            s += padding_size;
            fwrite(bp, padding_size, 1, fp);
            encoded_size += padding_size;
        }
    }

    printf(" encoded_size: %zu\n", encoded_size);

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
    printf("decoded size(%zu), string: %s\n", decoded_size, secret);

    return decoded_size;
}


size_t stegatzy_by_lsb(FILE *fp, const char *s)
{
    printf("stegatzy_by_lsb\n");

    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    read_bitmap_file(fp, bmp);
    size_t available_encode_size = get_encode_size(bmp, ENC_TYPE_LSB);

    printf(" available_encode_size: [%zu] bits\n", available_encode_size);
    printf(" [%zu] bits/[%zu] bytes required to encode %s\n", (strlen(s)+1)*8, strlen(s)+1, s);
    /* +1 byte, at the beginning sequence, to store the number of bytes to decipher */
    uint8_t **dest = malloc((strlen(s)+1) * sizeof(uint8_t *));

    /*
       TODO: Ways to reduce noise (eliminate non-hidden text)
       1) store the total secret bytes encoded at first 32 bytes(1 bit each)
       2) \0 char, maybe? but potentially run into coincident case
       3) smart pattern to truncate white space, i.e., too many unprintable chars ?
     */

    /* str_to_binary turn string `s' into binary form, 2D array */
    str_to_binary(s, dest);

    /* set `fp' offset point to start of pixel offset */
    int ret;
    if ((ret = fseek(fp, bmp->header.pixel_offset, SEEK_SET))) {
        ERROR_PRINT_ERR
        return -1;
    }


    int i, j;
    byte b;     /* temporary byte to read & write the LSBit */
    long pos;   /* store current fp cursor offset */
    for (i = 0; i < strlen(s); ++i) {
        for (j = 0; j <= 7; ++j) {
            pos = ftell(fp);
            fread(&b, sizeof(byte), 1, fp);
            set_bit(&b, 0, dest[i][j]);
            fseek(fp, pos, SEEK_SET);   /* rewind the fp cursor pos */
            fwrite(&b, sizeof(byte), 1, fp);
        }
    }

    return 0;
}


size_t stegatzy_decode_lsb(FILE *fp)
{
    size_t decoded_size = 0;
    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    read_bitmap_file(fp, bmp);
    fseek(fp, bmp->header.pixel_offset, SEEK_SET);

    byte bp;   /* temporary byte to hold a byte to retrieve the LSBit */
    byte bq;   /* temporary byte to hold the 8 retrieved bits before concat to secret string `ssp' */
    char *ss, *ssp;     /* pointer to the secret string */
    uint8_t bitpos = 0; /* current bit position offset */

    ss = malloc((sizeof(t_pixel) * bmp->info_header.width * bmp->info_header.height) / 8);
    ssp = ss;
    printf("(%d) malloc_usable_size(ss): %zu\n", __LINE__, malloc_usable_size(ss));
    while (fread(&bp, sizeof(byte), 1,fp)) {
        uint8_t bit = get_bit(&bp, 0);
        set_bit(&bq, bitpos++, bit);
        if (bitpos > 7) {
            *ss = (char)bq;
            ++ss;
            bitpos = 0;
            ++decoded_size;
        }
    }
    ss = ssp;   /* reset the `ss' pointer cursor to the beginning */
    printf("decoded size(%zu), string: %s\n", decoded_size, ss);

    return decoded_size;
}


size_t get_encode_size(t_bitmap *bmp, char enc_type)
{
    switch (enc_type) {
        case ENC_TYPE_PAD:
            return padding_check(bmp) * bmp->info_header.height;
            break;
        case ENC_TYPE_LSB:
            return get_pixel_array_size(bmp->info_header.width, bmp->info_header.height);
            break;
        default:
            return 0;
    }
}


/* set|toggle bit `b' bit at position `i' to `b':
 * [7|6|5|4|3|2|1|0]
 *                ^ <- starting index/position
 * return 0 on success, non-zero on error
 */
int set_bit(byte *bp, uint8_t i, uint8_t b)
{
    if (b == 1)
        *bp |= 1 << i;
    else if (b == 0)
        *bp &= ~(1 << i);
    else
        return -1;
    return 0;
}


uint8_t get_bit(byte *bp, uint8_t i)
{
    if (i < 0 || i > 7)
        return -1;
    return !!(*bp & (1 << i));
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


void str_to_binary(const char *s, uint8_t **dest)
{
    int i, j;
    i = 0;
    while (*s != '\0' && *s) {
        dest[i] = malloc(sizeof(uint8_t)*8);
        for (j = (sizeof(uint8_t)*8) - 1; j >= 0; --j)
            dest[i][j] = (*s & (1 << j)) ? 1 : 0;
        ++i; s++;
    }
}


void char_to_binary(char c, uint8_t *dest)
{
    int i;
    for (i = (sizeof(char)*8) - 1; i >= 0; --i)
        dest[i] = (c & (1 << i)) ? 1 : 0;
}


int *showbits(unsigned int x)
{
    int i, *bits;
    bits = malloc(sizeof(int) * 8);
    for (i = (sizeof(int)*8) - 1; i >= 0; --i) {
        bits[i] = (x & (1 << i)) ? 1 : 0;
        printf("%d", bits[i]);
    } printf("\n");

    return bits;
}

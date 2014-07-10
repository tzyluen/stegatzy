#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <malloc.h>
#include "tzybitmap.h"
#include "tzywav.h"
#include "stegatzylib.h"

size_t stegatzy_bmp_by_padding(FILE *fp, const char *s)
{
    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    read_bitmap_file(fp, bmp);
    size_t padding_size = padding_check(bmp);
    size_t offset = get_rowsize(bmp->info_header.width) - padding_size;
    size_t available_encode_size = get_bmp_encode_size(bmp, ENC_TYPE_PAD);

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


size_t stegatzy_bmp_decode_padding(FILE *fp)
{
    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    read_bitmap_file(fp, bmp);
    size_t padding_size = padding_check(bmp);
    size_t offset = get_rowsize(bmp->info_header.width) - padding_size;
    size_t available_encode_size = get_bmp_encode_size(bmp, ENC_TYPE_PAD);
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


size_t stegatzy_bmp_by_lsb(FILE *fp, const char *s)
{
    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    read_bitmap_file(fp, bmp);
    size_t available_encode_size = get_bmp_encode_size(bmp, ENC_TYPE_LSB);
    size_t encoded_size = 0;
    int ret;

    printf(" available_encode_size: [%zu] bits\n", available_encode_size);
    printf(" [%zu] bits required to encode %s\n", (strlen(s)+1)*8, s);

    /**
     *  TODO: Ways to reduce noise (eliminate non-hidden text)
     *  1) store the total secret bytes encoded at first 32 bytes(1 bit each)
     *  2) \0 char, maybe? but potentially run into coincident case
     *  3) smart pattern to truncate white space, i.e., too many unprintable chars ?
     */

    /* store the number of bytes to cipher, pixel's data first 32 bytes sequence(1 bit each) */
    size_t slen = strlen(s);
    if ((ret = fseek(fp, bmp->header.pixel_offset, SEEK_SET))) {
        ERROR_PRINT_ERR
        ret = -1;
        goto free_mem;  /* contrary to popular belief, goto ain't that bad ... */
    }

    printf("(%d) encoding data size offset: %lu\n", __LINE__, ftell(fp));

    byte b;     /* temporary byte to read & write the LSBit */
    long pos;   /* store current fp cursor offset */
    int i, j, bitpos; /* indices */
    for (bitpos = 32 - 1; bitpos >= 0; --bitpos) {
        pos = ftell(fp);
        fread(&b, sizeof(byte), 1, fp);
        set_bit(&b, 0, !!(slen & (1 << bitpos)));
        fseek(fp, pos, SEEK_SET);   /* rewind the fp cursor pos */
        fwrite(&b, sizeof(byte), 1, fp);
    }
 
    printf("(%d) sizeof(size_t)*8: %zu\n", __LINE__, sizeof(size_t) * 8 - 1);
    printf("(%d) encoded data offset: %lu\n", __LINE__, ftell(fp));

    /* str_to_binary turn string `s' into binary form, 2D array */
    uint8_t **dest = malloc((slen+1) * sizeof(uint8_t *));
    str_to_binary(s, dest);
    b = 0;
    pos = 0;
    for (i = 0; i < slen; ++i) {
        for (j = 0; j <= 7; ++j) {
            pos = ftell(fp);
            fread(&b, sizeof(byte), 1, fp);
            set_bit(&b, 0, dest[i][j]);
            fseek(fp, pos, SEEK_SET);
            fwrite(&b, sizeof(byte), 1, fp);
        }
        ++encoded_size;
    }
    ret = encoded_size;
    goto free_mem;

free_mem:
    if (dest)
        free(dest);
    free(bmp);

    return ret;
}


size_t stegatzy_bmp_decode_lsb(FILE *fp)
{
    size_t decoded_size;
    uint32_t encoded_size;
    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    read_bitmap_file(fp, bmp);
    fseek(fp, bmp->header.pixel_offset, SEEK_SET);

    byte bp;   /* temporary byte to hold a byte to retrieve the LSBit */
    byte bq;   /* temporary byte to hold the 8 retrieved bits before concat to secret string `ssp' */
    char *ss, *ssp;     /* pointer to the secret string */
    int8_t bitpos;     /* current bit position offset */

    /* read the first 32 bytes to get total encoded secret bytes */
    printf("(%d) encoded data size offset: %lu\n", __LINE__, ftell(fp));
    encoded_size = 0;
    for (bitpos = 31; bitpos >= 0; --bitpos) {
        fread(&bp, sizeof(byte), 1, fp);
        uint8_t bit = get_bit(&bp, 0);
        set_bit32(&encoded_size, bitpos, bit);
    }
    printf("(%d) encoded_size: %"PRId32"\n", __LINE__, encoded_size);

    ss = malloc((sizeof(t_pixel) * bmp->info_header.width * bmp->info_header.height) / 8);
    ssp = ss;
    printf("(%d) malloc_usable_size(ss): %zu\n", __LINE__, malloc_usable_size(ss));
    bitpos = 0;
    
    printf("(%d) encoded data offset: %lu\n", __LINE__, ftell(fp));
    decoded_size = 0;
    while (decoded_size < encoded_size) {
        fread(&bp, sizeof(byte), 1, fp);
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


size_t get_bmp_encode_size(t_bitmap *bmp, char enc_type)
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


size_t stegatzy_wav_by_lsb(FILE *fp, const char *s, const char *ofn)
{
    int ret = 0;
    t_wav *wav_f = malloc(sizeof(t_wav));
    if ((ret = read_wav_file(fp, wav_f)))
        ERROR_PRINT_ERR;
    //print_wav_info(wav_f);
    size_t encoded_size = 0;

    FILE *sfp = fopen(ofn, "w+");
    /* write the wav file header */
    char *header = malloc(PCM_DATA_HEADER_OFFSET);  // first 44 bytes; FIXME: Non-PCM different offset
    rewind(fp);
    fread(header, 1, PCM_DATA_HEADER_OFFSET, fp);
    fwrite(header, 1, PCM_DATA_HEADER_OFFSET, sfp);

    /* write/store the number of bytes to cipher, pixel's data first 32 bytes sequence(1 bit each) */
    size_t slen = strlen(s);
    byte b;     /* temporary byte to read & write the LSBit */
    int i, j, bitpos;   /* indices */
    int k = 0;          /* store the bytes used(offset) for encoding */
    unsigned char low = 0;  /* low order byte: true|false */
    for (bitpos = 32 - 1; bitpos >= 0; /* NOTE: empty */ ) {
        b = *wav_f->sampled_data;

        if (low) {
            set_bit(&b, 0, !!(slen & (1 << bitpos)));
            --bitpos;
        }

        fwrite(&b, sizeof(b), 1, sfp);
        ++wav_f->sampled_data;
        ++k;
        low = !low;
    }

    /* write the wav data contents */
    uint8_t **dest = malloc(slen * sizeof(uint8_t *));
    str_to_binary(s, dest);

    uint8_t s_sample;
    for (i = 0; i < slen; ++i) {
        j = 0;
        while ( (j <= 7) && (k < wav_f->header.subchunk2_size) ) {
            s_sample = *wav_f->sampled_data;

            if (low) {
                //printf("[BEFORE] %d, %#010x\n", k, s_sample);
                set_bit(&s_sample, 0, dest[i][j]);
                //printf("[ AFTER] %d, %#010x\n", k, s_sample);
                ++j;
            }

            fwrite(&s_sample, 1, sizeof(s_sample), sfp);
            ++wav_f->sampled_data;
            ++k;
            low = !low;
        }
        ++encoded_size;
    }
    ret = encoded_size; //FIXME: fully utilize `ret' for proper return val

    /* write the remaining chunk data */
    fwrite(wav_f->sampled_data, 1, wav_f->header.subchunk2_size - k, sfp);
    fseek(fp, wav_f->header.subchunk2_size, SEEK_CUR);

    /* write the remaining additional footer */
    b = 0;
    while (fread(&b, 1, sizeof(byte), fp) == sizeof(byte))
        fwrite(&b, 1, sizeof(byte), sfp);

    ERROR_PRINT_ERR
    fclose(sfp);

    return ret;
}


size_t stegatzy_wav_decode_lsb(FILE *fp)
{
    int ret = 0;
    t_wav *wav_f = malloc(sizeof(t_wav));
    if ((ret = read_wav_file(fp, wav_f)))
        return ret;

    byte bp;   /* temporary byte to hold a byte to retrieve the LSBit */
    byte bq;   /* temporary byte to hold the 8 retrieved bits before concat to secret string `ssp' */
    char *ss, *ssp;     /* pointer to the secret string */
    int8_t bitpos;      /* current bit position offset */
    unsigned char low = 0;  /* low order byte: true|false */
    size_t decoded_size;
    uint32_t encoded_size;

    fseek(fp, PCM_DATA_HEADER_OFFSET, SEEK_SET);

    /* read the first 64 bytes to get total encoded secret bytes */
    encoded_size = 0;
    for (bitpos = 31; bitpos >= 0; /* NOTE: empty */) {
        fread(&bp, sizeof(byte), 1, fp);

        if (low) {
            uint8_t bit = get_bit(&bp, 0);
            set_bit32(&encoded_size, bitpos, bit);
            --bitpos;
        }

        low = !low;
    }
    printf("(%d) encoded_size: %"PRId32"\n", __LINE__, encoded_size);

    //ss = malloc(wav_f->header.subchunk2_size / 2);
    ss = malloc(encoded_size + 1);
    ssp = ss;

    bp = bq = low = bitpos = decoded_size = 0;
    while (fread(&bp, 1, 1, fp) >= 1 && decoded_size <= encoded_size) {
        //printf("(%d) %#010x\n", __LINE__, bp);
        if (low) {
            //printf("(%d) %#010x\n", __LINE__, bp);
            //set_bit(&bq, bitpos, get_bit(&bp, 0));
            uint8_t bit = get_bit(&bp, 0);
            //printf("%d", bit);
            set_bit(&bq, bitpos, bit);
            ++bitpos;
            if (bitpos > 7) {
                *ss = bq;
                ++ss;
                bitpos = 0;     /* reset */
                //printf("%c\n", *ss);
                ++decoded_size;
            }
        }

        low = !low;
    }
    ss = ssp;
    printf(" %s\n", ss);

    return decoded_size;
}


/**
 * set|toggle bit `b' bit at position `i' to `b':
 * [7|6|5|4|3|2|1|0]
 *                ^- starting index/position
 *  return 0 on success, non-zero on error
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


int set_bit16(uint16_t *bp, uint8_t i, uint8_t b)
{
    if (b == 1)
        *bp |= 1 << i;
    else if (b == 0)
        *bp &= ~(1 << i);
    else
        return -1;
    return 0;
}


int set_bit32(uint32_t *bp, uint8_t i, uint8_t b)
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


void stream_bmp_padding_contents(FILE *fp, t_bitmap *bmp, size_t padding_size, size_t encode_size)
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


void print_signed_hex(int16_t i)
{
    printf("%c%s%04X\n", (i<0) ? '-':' ', "0x", (i<0)?-i:i);
}

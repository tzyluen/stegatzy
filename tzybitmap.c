#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include "tzybitmap.h"


void set_bitmap(t_bitmap *bmp, int i)
{
    printf("set_bitmap\n");
}


int set_bitmap_filename(t_bitmap *bmp, const char *s)
{
    char *new = malloc(sizeof(char) * strlen(s));
    if (!new)
        return ENOMEM;
    strcpy(new, s);
    bmp->name = new;

    return 0;
}


void unset_bitmap(t_bitmap *bmp, int i)
{
    printf("unset_bitmap\n");
}


int get_bitmap(t_bitmap *bmp, int i)
{
    return i;
}


t_bitmap *create_bitmap(const char *name, int width, int height, int depth, t_pixel rgb)
{
    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    if (set_bitmap_filename(bmp, name))
        ERROR_PRINT_ERR

    size_t pixsize = get_pixel_array_size(width, height);

    /* header */
    bmp->header.type = 0x4D42;      /* set to 'BM' */
    bmp->header.file_size = 54 + pixsize;
    bmp->header.reserved1 = 0;
    bmp->header.reserved2 = 0;
    bmp->header.pixel_offset = 54;

    /* info header */
    bmp->info_header.size = 40;
    bmp->info_header.width = width;
    bmp->info_header.height = height;
    bmp->info_header.color_planes = 1;
    bmp->info_header.bits_per_pixel = depth;
    bmp->info_header.compression = 0;
    bmp->info_header.image_size = pixsize;
    bmp->info_header.horizontal = 2835;     /* pixels/meter, resolution of image: */
    bmp->info_header.vertical = 2835;       /* 72 DPI x 39.3701 inches per meter yields 2834.6472 */
    bmp->info_header.color_palette = 0;

    /* TODO: instead of always allocate `pixsize', add additional checks to exclude the padding bytes,
     *       because `t_pixel' doesn't store padding bytes. We will pad the alignment bytes at
     *       function `size_t write_bitmap_pixel_data(t_bitmap *bmp, FILE *fp); '
     */
    t_pixel *pixel = malloc(pixsize);

    int h, w, z = 0;
    for (h = 0; h < height; ++h) {
        for (w = 0; w < width; ++w) {
            pixel[z].blue = rgb.blue;
            pixel[z].green = rgb.green;
            pixel[z].red = rgb.red;
            ++z;
        }
    }

    bmp->pixel = pixel;

    return bmp;
}


size_t write_bitmap_file(const char *name, t_bitmap *bmp)
{
    FILE *fp = fopen(name, "wb+");
    if (fp == NULL)
        ERROR_PRINT_ERR

    fwrite(&bmp->header.type, 2, 1, fp);
    fwrite(&bmp->header.file_size, 4, 1, fp);
    fwrite(&bmp->header.reserved1, 2, 1, fp);
    fwrite(&bmp->header.reserved2, 2, 1, fp);
    fwrite(&bmp->header.pixel_offset, 4, 1, fp);
    fwrite(&bmp->info_header.size, 4, 1, fp);
    fwrite(&bmp->info_header.width, 4, 1, fp);
    fwrite(&bmp->info_header.height, 4, 1, fp);
    fwrite(&bmp->info_header.color_planes, 2, 1, fp);
    fwrite(&bmp->info_header.bits_per_pixel, 2, 1, fp);
    fwrite(&bmp->info_header.compression, 4, 1, fp);
    fwrite(&bmp->info_header.image_size, 4, 1, fp);
    fwrite(&bmp->info_header.horizontal, 4, 1, fp);
    fwrite(&bmp->info_header.vertical, 4, 1, fp);
    fwrite(&bmp->info_header.color_palette, 4, 1, fp);
    fwrite(&bmp->info_header.colors_used, 4, 1, fp);

    write_bitmap_pixel_data(bmp, fp);

    fclose(fp);

    return 0;
}


/**
 * return value:
 * on success, return positive value - number of bytes written to *fp
 */
size_t write_bitmap_pixel_data(t_bitmap *bmp, FILE *fp)
{
    size_t padding_bytes, z = 0;
    char zero[] = {0,0,0};
    padding_bytes = padding_check(bmp);
    if (padding_bytes > 0) {   /* if padding is required */
        int h, w, i = 0;
        for (h = 0; h < bmp->info_header.height; ++h) {
            for (w = 0; w < bmp->info_header.width; ++w, z += BYTES_PER_PIXEL)
                fwrite(&bmp->pixel[i++], BYTES_PER_PIXEL, 1, fp);
            fwrite(&zero, sizeof(char), padding_bytes, fp);
            z += padding_bytes;
        }
    } else  /* no padding required, straight up write from pixel array */
        z = fwrite(&bmp->pixel, bmp->info_header.image_size, 1, fp);

    return z;
}


/**
 * return value:
 * on success, return 0
 * on error, return ENOENT if file isn't available, negative value if reading failed.
 */
int read_bitmap_file(FILE *fp, t_bitmap *bmp)
{
    int ret;
    if (fp == NULL)
        return ENOENT;
    if ((ret = read_bitmap_file_header(bmp, fp)))
        return ret;
    if (bmp->header.type != 0x4D42) /* if file ain't bitmap: BM */
        return EFTYPE;
    if ((ret = read_bitmap_info_header(bmp, fp)))
        return ret;
    if ((ret = read_bitmap_data(bmp, fp)))
        return ret;

    return 0;
}


/**
 * return value:
 * on success, return 0.
 * on error, return negative value.
 */
int read_bitmap_file_header(t_bitmap *bmp, FILE *img)
{
    //if (fread(&bmp->header, sizeof(unsigned char), sizeof(t_BITMAPFILEHEADER), img) < 1)
    //    return -1;    /* couldn't read the file header */

    int ret;
    if ((ret = fread(&bmp->header.type, sizeof(unsigned char), sizeof(uint16_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->header.file_size, sizeof(unsigned char), sizeof(uint32_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->header.reserved1, sizeof(unsigned char), sizeof(int16_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->header.reserved2, sizeof(unsigned char), sizeof(int16_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->header.pixel_offset, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;

    return 0;
}


/**
 * return value:
 * on success, return 0.
 * on error, return negative value.
 */
int read_bitmap_info_header(t_bitmap *bmp, FILE *img)
{
    //if (fread(&bmp->info_header, sizeof(unsigned char), sizeof(t_BITMAPINFOHEADER), img) < 1)
    //    return -1;    /* couldn't read the info header */
    
    int ret;
    if ((ret = fread(&bmp->info_header.size, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.width, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.height, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.color_planes, sizeof(unsigned char), sizeof(int16_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.bits_per_pixel, sizeof(unsigned char), sizeof(int16_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.compression, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.image_size, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.horizontal, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.vertical, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.color_palette, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;
    if ((ret = fread(&bmp->info_header.colors_used, sizeof(unsigned char), sizeof(int32_t), img)) < 1)
        return ret;

    return 0;
}


/**
 * return value:
 * on success, return 0.
 * on error, return ENOMEM if failed to allocate memory. return EOVERFLOW if px memory overflow.
 *           return if fread read size not equal to bitmap image size,
 */
int read_bitmap_data(t_bitmap *bmp, FILE *img)
{
    int ret;
    t_pixel *px = malloc(bmp->info_header.image_size);
    if (px == NULL)
        return ENOMEM;
    if ((ret = fread(px, sizeof(unsigned char), bmp->info_header.image_size, img)) < bmp->info_header.image_size)
        ret = EOVERFLOW;
    else if (ret != bmp->info_header.image_size) {
        ERROR_PRINT_ERR
        return ret;
    }

    bmp->pixel = px;

    return 0;
}


int hide(t_bitmap *bmp, const char *s)
{
    printf("hide\n");
    return 0;
}


int unhide(t_bitmap *bmp)
{
    printf("unhide\n");
    return 0;
}


/* return row size in bytes required */
size_t get_rowsize(int width)
{
    return ((BITS_PER_PIXEL*width)+31)/32*4;
}


/* return total pixel storage size in bytes */
size_t get_pixel_array_size(int width, int height)
{
    return get_rowsize(width)*height;
}


/* return total bytes of padding required per row(or width) */
size_t padding_check(t_bitmap *bmp)
{
    return get_rowsize(bmp->info_header.width) - (BYTES_PER_PIXEL * bmp->info_header.width);
}


void streamout_bitmap(t_bitmap *bmp)
{
    printf("Image: %s\n"
           "===============================================\n"
           "header->type: %x\n"
           "header->file_size: %"PRId32"\n"
           "header->reserved1: %d\n"
           "header->reserved2: %d\n"
           "header->pixel_offset: %"PRId32"\n",
            bmp->name, bmp->header.type,
            bmp->header.file_size, bmp->header.reserved1,
            bmp->header.reserved2, bmp->header.pixel_offset);

    printf("===============================================\n"
           "info_header->size: %"PRId32"\n"
           "info_header->width: %"PRId32"\n"
           "info_header->height: %"PRId32"\n"
           "info_header->color_planes: %d\n"
           "info_header->bits_per_pixel: %d\n"
           "info_header->compression: %"PRId32"\n"
           "info_header->image_size: %"PRId32"\n"
           "info_header->horizontal: %"PRId32"\n"
           "info_header->vertical: %"PRId32"\n"
           "info_header->color_palette: %"PRId32"\n"
           "info_header->colors_used: %"PRId32"\n",
            bmp->info_header.size, bmp->info_header.width,
            bmp->info_header.height, bmp->info_header.color_planes,
            bmp->info_header.bits_per_pixel, bmp->info_header.compression,
            bmp->info_header.image_size, bmp->info_header.horizontal,
            bmp->info_header.vertical, bmp->info_header.color_palette,
            bmp->info_header.colors_used);

    printf("===============================================\n");
    size_t i;
    for (i = 0; i < bmp->info_header.image_size; ++i) {
        //printf("(%zu): %x, %x, %x\n", i, bmp->pixel[i].blue, bmp->pixel[i].green, bmp->pixel[i].red);
    }
}

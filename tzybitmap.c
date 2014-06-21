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
    char *new = (char *)malloc(sizeof(char) * strlen(s));
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


t_bitmap *create_bitmap(int n)
{
    return (t_bitmap *)malloc((n+7)/8);
}


/**
 * return value:
 * on success, return 0
 * on error, return ENOENT if file isn't available, negative value if reading failed.
 */
int read_bitmap_file(const char *file, t_bitmap *bmp)
{
    int ret;
    FILE *bmpfile = fopen(file, "rb");
    if (bmpfile == NULL)
        return ENOENT;
    if ((ret = read_bitmap_file_header(bmp, bmpfile)))
        return ret;
    if (bmp->header.type != 0x4D42) /* if file ain't bitmap: BM */
        return EFTYPE;
    if ((ret = read_bitmap_info_header(bmp, bmpfile)))
        return ret;
    if ((ret = read_bitmap_data(bmp, bmpfile)))
        return ret;
    if (bmp->name == '\0')
        set_bitmap_filename(bmp, file);

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
    t_pixel **px = malloc(bmp->info_header.image_size);
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
}

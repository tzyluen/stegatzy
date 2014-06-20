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
        return -1;
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


int read_bitmap_file(const char *file, t_bitmap *bmp)
{
    int error = 0;
    FILE *bmpfile = fopen(file, "rb");
    if (bmpfile == NULL)
        return -1;
    if (read_bitmap_file_header(bmp, bmpfile))
        error = -1;

    /* if file ain't bitmap: BM */
    if (bmp->header.type != 0x4D42)
        error = -2;

    if (read_bitmap_info_header(bmp, bmpfile))
        error = -1;
    if (read_bitmap_data(bmp, bmpfile))
        error = -1;

    if (bmp->name == '\0') {
        //printf("bmp->name: %s\n", bmp->name);
        set_bitmap_filename(bmp, file);
    }

    return error;
}


int read_bitmap_file_header(t_bitmap *bmp, FILE *img)
{
    //if (fread(&bmp->header, sizeof(unsigned char), sizeof(t_BITMAPFILEHEADER), img) < 1)
    //    return -1;    /* couldn't read the file header */

    if (fread(&bmp->header.type, sizeof(unsigned char), sizeof(uint16_t), img) < 1)
        return -1;
    if (fread(&bmp->header.file_size, sizeof(unsigned char), sizeof(uint32_t), img) < 1)
        return -1;
    if (fread(&bmp->header.reserved1, sizeof(unsigned char), sizeof(int16_t), img) < 1)
        return -1;
    if (fread(&bmp->header.reserved2, sizeof(unsigned char), sizeof(int16_t), img) < 1)
        return -1;
    if (fread(&bmp->header.pixel_offset, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;

    return 0;
}


int read_bitmap_info_header(t_bitmap *bmp, FILE *img)
{
    //if (fread(&bmp->info_header, sizeof(unsigned char), sizeof(t_BITMAPINFOHEADER), img) < 1)
    //    return -1;    /* couldn't read the info header */
    
    if (fread(&bmp->info_header.size, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.width, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.height, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.color_planes, sizeof(unsigned char), sizeof(int16_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.bits_per_pixel, sizeof(unsigned char), sizeof(int16_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.compression, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.image_size, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.horizontal, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.vertical, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.color_palette, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;
    if (fread(&bmp->info_header.colors_used, sizeof(unsigned char), sizeof(int32_t), img) < 1)
        return -1;

    return 0;
}


int read_bitmap_data(t_bitmap *bmp, FILE *img)
{
    t_pixel **px = malloc(bmp->info_header.image_size);
    if (px == NULL ) {
        ERROR_PRINT_ERR;
        return -1;
    }

    /*if (fread(*px, sizeof(t_pixel), bmp->info_header.image_size, img) != bmp->info_header.image_size) {
        ERROR_PRINT_ERR;
        return -1;
    }*/

    //**pixel = malloc(data_size)
    //     return -1;

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

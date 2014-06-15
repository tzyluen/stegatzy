#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "tzybitmap.h"


void set_bitmap(t_bitmap *bmp, int i)
{
    printf("set_bitmap\n");
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
        error = -1;
    if (read_bitmap_file_header(bmp, bmpfile))
        error = -1;
    if (read_bitmap_info_header(bmp, bmpfile))
        error = -1;
    if ((!error) && strlen(bmp->name) <= 0)
        strcpy(bmp->name, file);

    return error;
}


int read_bitmap_file_header(t_bitmap *bmp, FILE *img)
{
    //size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
    if (fread(&bmp->header, sizeof(unsigned char), sizeof(t_BITMAPFILEHEADER), img) < 1)
        return -1;    /* couldn't read the file header */
    return 0;
}


int read_bitmap_info_header(t_bitmap *bmp, FILE *img)
{
    if (fread(&bmp->info_header, sizeof(unsigned char), sizeof(t_BITMAPINFOHEADER), img) < 1)
        return -1;    /* couldn't read the info header */
    return 0;
}


void streamout_bitmap(t_bitmap *bmp)
{
    printf("Image: %s\n"
           "header->sig_b: %d\n"
           "header->sig_m: %d\n"
           "header->file_size: %d\n"
           "header->reserved1: %d\n"
           "header->reserved2: %d\n"
           "header->pixel_offset: %d\n",
            bmp->name, bmp->header.sig_b, bmp->header.sig_m,
            bmp->header.file_size, bmp->header.reserved1,
            bmp->header.reserved2, bmp->header.pixel_offset);

    printf("info_header->size: %d\n"
           "info_header->width: %d\n"
           "info_header->height: %d\n"
           "info_header->color_planes: %d\n"
           "info_header->bits_per_pixel: %d\n"
           "info_header->zeros: ",
            bmp->info_header.size, bmp->info_header.width,
            bmp->info_header.height, bmp->info_header.color_planes,
            bmp->info_header.bits_per_pixel);

    int i;
    for (i = 0; i < sizeof(bmp->info_header.zeros); ++i ) {
        printf("%d%c", bmp->info_header.zeros[i], (i+1 > sizeof(bmp->info_header.zeros)) ? '\n' : ' ');
    }
}

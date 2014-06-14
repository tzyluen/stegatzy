#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
    read_bitmap_file_header(bmp, bmpfile);
    read_bitmap_info_header(bmp, bmpfile);

    return error;
}


void read_bitmap_file_header(t_bitmap *bmp, FILE *img)
{
    //size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
    fread(&bmp->header, sizeof(unsigned char), sizeof(t_BITMAPFILEHEADER), img);
}


void read_bitmap_info_header(t_bitmap *bmp, FILE *img)
{
    fread(&bmp->info_header, sizeof(unsigned char), sizeof(t_BITMAPINFOHEADER), img);
}


void streamout_bitmap(t_bitmap *bmp)
{
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tzybitmap.h"


void set_bitmap(t_bitmap bmp, int i)
{
    printf("set_bitmap\n");
}


void unset_bitmap(t_bitmap bmp, int i)
{
    printf("unset_bitmap\n");
}


int get_bitmap(t_bitmap bmp, int i)
{
    return 0;
}


t_bitmap *create_bitmap(int n)
{
    return (t_bitmap *)malloc((n+7)/8);
}

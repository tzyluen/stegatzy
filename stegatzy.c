#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tzybitmap.h"

int main(int argc, char **argv)
{
    //t_bitmap *bmp = create_bitmap(150);
    //set_bitmap(bmp, 1);
    //unset_bitmap(bmp, 1);
    //int i = get_bitmap(bmp, 1);
    //printf("get_bitmap(bmp, 1): %d\n", i);
    t_bitmap *bmp = (t_bitmap *)malloc(sizeof(t_bitmap));
    int ret = read_bitmap_file("brucelee.bmp", bmp);
    if (ret)
        printf("-E- error code: %d\n", ret);

    streamout_bitmap(bmp);

    return EXIT_SUCCESS;
}

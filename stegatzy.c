#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tzybitmap.h"

int main(int argc, char **argv)
{
    t_bitmap *bmp = create_bitmap(150);
    set_bitmap(bmp, 1);
    unset_bitmap(bmp, 1);
    int i = get_bitmap(bmp, 1);
    printf("get_bitmap(bmp, 1): %d\n", i);
    int ret = read_bitmap_file("./brucelee.bmp", bmp);
    printf("read_bitmap_file: %d\n", ret);

    return EXIT_SUCCESS;
}

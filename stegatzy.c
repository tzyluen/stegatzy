#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>      /* refer to /usr/include/asm-generic/errno.h */
#include "tzybitmap.h"

#define BITMAP_FILE        1    /* argv index key for target bitmap file */
#define SECRET_TEXT        2    /* argv index key for target secret text */

void test_create_bitmap();
void usage();

int main(int argc, char **argv)
{
    if (argc > 3 || argc < 2)
        usage();

    t_bitmap *bmp = malloc(sizeof(t_bitmap));
    int ret = 0;
    if ((ret = read_bitmap_file(argv[BITMAP_FILE], bmp))) {
        ERROR_PRINT_ERR;
    }

    if (argc == 3)  /* hide */
        hide(bmp, argv[SECRET_TEXT]);
    if (argc == 2)  /* unhide */
        unhide(bmp);

    //streamout_bitmap(bmp);

    test_create_bitmap();

    return EXIT_SUCCESS;
}


void test_create_bitmap()
{
    int ret = 0;
    printf("===============================================\n");
    t_bitmap *bmp = create_bitmap("tzy", 201, 201, 24);
    write_bitmap_file("tzy.bmp", bmp);
    free(bmp);

    bmp = malloc(sizeof(t_bitmap)); if ((ret = read_bitmap_file("tzy.bmp", bmp)))
        ERROR_PRINT_ERR;

    streamout_bitmap(bmp);
}


void usage()
{
    printf("usage:\nstegatzy <bitmap file> \"secret message\"\n");
    exit(1);
}

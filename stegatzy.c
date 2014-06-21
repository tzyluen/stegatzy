#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>      /* refer to /usr/include/asm-generic/errno.h */
#include "tzybitmap.h"

void usage();

int main(int argc, char **argv)
{
    if (argc > 3 || argc < 2)
        usage();

    t_bitmap *bmp = (t_bitmap *)malloc(sizeof(t_bitmap));
    int ret = 0;
    if ((ret = read_bitmap_file(argv[1], bmp))) {
        ERROR_PRINT_ERR;
    }

    if (argc == 3)  /* hide */
        hide(bmp, argv[2]);
    if (argc == 2)  /* unhide */
        unhide(bmp);
    
    streamout_bitmap(bmp);

    return EXIT_SUCCESS;
}


void usage()
{
    printf("usage:\nstegatzy <bitmap file> \"secret message\"\n");
    exit(1);
}

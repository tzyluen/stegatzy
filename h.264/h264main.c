#include <stdio.h>
#include "tzyh264.h"

void usage();

int main(int argc, char **argv)
{
    if (argc != 2) {
        usage();
        return 1;
    }

    read_h264_file(argv[1]);

    return 0;
}


void usage()
{
    printf("usage: h264main <input file>\n");
}

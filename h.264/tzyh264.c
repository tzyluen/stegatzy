#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include "tzyh264.h"

int read_h264_file(const char *f)
{
    int ret;
    AVFormatContext *fmt_ctx = NULL;
    AVDictionaryEntry *tag = NULL;

    av_register_all();
    if ((ret = avformat_open_input(&fmt_ctx, f, NULL, NULL)))
        return ret;

    while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
        printf("%s=%s\n", tag->key, tag->value);

    avformat_close_input(&fmt_ctx);

    return 0;
}

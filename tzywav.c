/**
 * Stegatzy
 * Copyright (C) 2014 Ng Tzy Luen. All Rights Reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include "tzywav.h"

/**
 * return value:
 * on success, return 0
 * on error, return -ENOENT if file isn't available, negative value if reading failed.
 */
int read_wav_file(FILE *fp, t_wav *wav_f)
{
    int ret;
    if (fp == NULL)
        return -ENOENT;
    if ((ret = read_wav_header(wav_f, fp)))
        return ret;
    if ((ret = read_wav_data(wav_f, fp)))
        return ret;
    return 0;
}


/**
 * return value:
 * on success, return 0
 * on error, return negative value
 */
int read_wav_header(t_wav *wav_f, FILE *fp)
{
    int ret;
    if ((ret = fread(&wav_f->header, 1, sizeof(t_WAVHEADER), fp)) < 0)
        return ret;
    return 0;
}


/**
 * return value:
 * on success, return 0
 * on error, return negative value
 */
int read_wav_data(t_wav *wav_f, FILE *fp)
{
    int ret;
    uint8_t *sampled_data = malloc(wav_f->header.subchunk2_size);
    if (sampled_data == NULL)
        return -ENOMEM;
    if ((ret = fread(sampled_data, 1, wav_f->header.subchunk2_size, fp)) < wav_f->header.subchunk2_size)
        return -EOVERFLOW;
    else if (ret != wav_f->header.subchunk2_size)
        return ret;

    wav_f->sampled_data = sampled_data;

    return 0;
}


void print_wav_info(t_wav *wav_f)
{
    printf("Wav:\n"
           "header->chunk_id: %#010x\n"
           "header->chunk_size: %"PRId32"\n"
           "header->format: %#010x\n"
           "header->subchunk1_id: %#010x\n"
           "header->subchunk1_size: %"PRId32"\n"
           "header->audio_format: %"PRId16"\n"
           "header->num_channels: %"PRId16"\n"
           "header->sample_rate: %"PRId32"\n"
           "header->byte_rate: %"PRId32"\n"
           "header->block_align: %"PRId16"\n"
           "header->bits_per_sample: %"PRId16"\n"
           "header->subchunk2_id: %#010x\n"
           "header->subchunk2_size: %"PRId32"\n",
            wav_f->header.chunk_id,
            wav_f->header.chunk_size,
            wav_f->header.format,
            wav_f->header.subchunk1_id,
            wav_f->header.subchunk1_size,
            wav_f->header.audio_format,
            wav_f->header.num_channels,
            wav_f->header.sample_rate,
            wav_f->header.byte_rate,
            wav_f->header.block_align,
            wav_f->header.bits_per_sample,
            wav_f->header.subchunk2_id,
            wav_f->header.subchunk2_size);
}

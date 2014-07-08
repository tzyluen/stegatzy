#ifndef TZYWAV_H
#define TZYWAV_H

typedef uint8_t byte;

/**
 * Wav file format reference, https://ccrma.stanford.edu/courses/422/projects/WaveFormat:
 */
typedef struct {
    uint32_t chunk_id;           /* offset:0, size:4 bytes */
    uint32_t chunk_size;         /* offset:4, size:4 bytes */
    uint32_t format;             /* offset:8, size:4 bytes */
    uint32_t subchunk1_id;       /* offset:12, size:4 bytes */
    uint32_t subchunk1_size;     /* offset:16, size:4 bytes */
    uint16_t audio_format;       /* offset:20, size:2 bytes */
    uint16_t num_channels;       /* offset:22, size:2 bytes */
    uint32_t sample_rate;        /* offset:24, size:4 bytes */
    uint32_t byte_rate;          /* offset:28, size:4 bytes */
    uint16_t block_align;        /* offset:32, size:2 bytes */
    uint16_t bits_per_sample;    /* offset:32, size:2 bytes */
    uint32_t subchunk2_id;       /* offset:36, size:4 bytes */
    uint32_t subchunk2_size;     /* offset:40, size:4 bytes */
} t_WAVHEADER;

typedef struct {
    t_WAVHEADER header;
    byte **sampled_data;
} t_wav;


int read_wav_file(FILE *, t_wav *);
int read_wav_header(t_wav *, FILE *);
int read_wav_data(t_wav *, FILE *);
void print_wav_info(t_wav *);

#endif

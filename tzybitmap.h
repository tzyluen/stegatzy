#ifndef TZYBITMAP_H
#define TZYBITMAP_H
/**
 * Bitmap file format reference, http://en.wikipedia.org/wiki/BMP_file_format
 */
typedef uint8_t byte;

/* BITMAPFILEHEADER structure */
typedef struct {
    byte        sig_b;
    byte        sig_m;
    int32_t     file_size;
    int16_t     reserved1;
    int16_t     reserved2;
    int32_t     pixel_offset;
} t_BITMAPFILEHEADER;

/* BITMAPINFOHEADER structure */
typedef struct {
    int32_t     size;
    int32_t     width;
    int32_t     height;
    int32_t     color_planes;
    int16_t     bits_per_pixel;
    int16_t     zeros[24];
} t_BITMAPINFOHEADER;

typedef struct {
    byte blue;
    byte green;
    byte red;
} t_pixel;

/** A bitmap image consists of the BITMAPFILEHEADER and BITMAPINFOHEADER structures,
 *  and the pixel array.
 */
typedef struct {
    t_BITMAPFILEHEADER      header;
    t_BITMAPINFOHEADER      info_header;
    t_pixel                 *pixel;
} t_bitmap;

void set_bitmap(t_bitmap *, int);
void unset_bitmap(t_bitmap *, int);
int get_bitmap(t_bitmap *, int);
t_bitmap *create_bitmap(int);
void read_bitmap_file_header(t_bitmap *, FILE *);
void read_bitmap_info_header(t_bitmap *, FILE *);
void streamout_bitmap(t_bitmap *);
int read_bitmap_file(const char *, t_bitmap *);

#endif

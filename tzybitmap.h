#ifndef TZYBITMAP_H
#define TZYBITMAP_H
/**
 * Bitmap file format reference, http://en.wikipedia.org/wiki/BMP_file_format
 */
typedef uint8_t byte;

/* BITMAPFILEHEADER structure */
typedef struct {
    byte sig_b;             /* offset:0, size:2 bytes */
    byte sig_m;             /* types: {BM, BA, CI, CP, IC, PT} */
    uint32_t file_size;     /* offset:2, size:4 bytes, bmp filesize in bytes */
    int16_t reserved1;
    int16_t reserved2;
    int32_t pixel_offset;   /* offset:10, size:4 bytes, bmp image starting address (pixel array) */
} t_BITMAPFILEHEADER;

/* BITMAPINFOHEADER structure */
typedef struct {
    int32_t size;           /* offset:14, size:4 bytes, size of this header */
    int32_t width;          /* offset:18, size:4 bytes, bmp width in pixel (signed int) */
    int32_t height;         /* offset:22, size:4 bytes, bmp height in pixel (signed int) */  
    int16_t color_planes;   /* offset:26, size:2 bytes, the number of color planes must be 1 */
    int16_t bits_per_pixel; /* offset:28, size:2 bytes, bits per pixel, i.e, 1,4,8,16,24,32 */
    int32_t compression;    /* offset:30, size:4 bytes, compression method */
    int32_t image_size;     /* offset:34, size:4 bytes, size of the raw bmp data */
    int32_t horizontal;     /* offset:38, size:4 bytes, horizontal resolution of the image */
    int32_t vertical;       /* offset:42, size:4 bytes, vertical resolution of the image */
    int32_t color_palette;  /* offset:46, size:4 bytes, number of colors in color palette */
    int32_t colors_used;    /* offset:50, size:4 bytes, number of important colors used */
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
    t_BITMAPFILEHEADER header;
    t_BITMAPINFOHEADER info_header;
    t_pixel            *pixel;
    char               *name;
} t_bitmap;

void set_bitmap(t_bitmap *, int);
void unset_bitmap(t_bitmap *, int);
int get_bitmap(t_bitmap *, int);
t_bitmap *create_bitmap(int);
int read_bitmap_file_header(t_bitmap *, FILE *);
int read_bitmap_info_header(t_bitmap *, FILE *);
void streamout_bitmap(t_bitmap *);
int read_bitmap_file(const char *, t_bitmap *);

#endif

#ifndef TZYBITMAP_H
#define TZYBITMAP_H
/**
 * Bitmap file format reference, http://en.wikipedia.org/wiki/BMP_file_format
 */

#define E_BMP_INVALID_FILE   -2
#define E_BMP_READ_ERROR     -1


typedef uint8_t byte;

/* BITMAPFILEHEADER structure */
typedef struct {
    uint16_t type;           /* offset:0, size:2 bytes, types: {BM, BA, CI, CP, IC, PT} */
    uint32_t file_size;      /* offset:2, size:4 bytes, bmp filesize in bytes */
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixel_offset;   /* offset:10, size:4 bytes, bmp image starting address (pixel array) */
} t_BITMAPFILEHEADER;

/* BITMAPINFOHEADER structure */
typedef struct {
    uint32_t size;           /* offset:14, size:4 bytes, size of this(DIB) header */
    uint32_t width;          /* offset:18, size:4 bytes, bmp width in pixel (signed int) */
    uint32_t height;         /* offset:22, size:4 bytes, bmp height in pixel (signed int) */  
    uint16_t color_planes;   /* offset:26, size:2 bytes, the number of color planes must be 1 */
    uint16_t bits_per_pixel; /* offset:28, size:2 bytes, bits per pixel, i.e, 1,4,8,16,24,32 */
    uint32_t compression;    /* offset:30, size:4 bytes, compression method */
    uint32_t image_size;     /* offset:34, size:4 bytes, size of the raw bmp data */
    uint32_t horizontal;     /* offset:38, size:4 bytes, horizontal resolution of the image */
    uint32_t vertical;       /* offset:42, size:4 bytes, vertical resolution of the image */
    uint32_t color_palette;  /* offset:46, size:4 bytes, number of colors in color palette */
    uint32_t colors_used;    /* offset:50, size:4 bytes, number of important colors used */
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
int set_bitmap_filename(t_bitmap *, const char *);
void unset_bitmap(t_bitmap *, int);
int get_bitmap(t_bitmap *, int);
t_bitmap *create_bitmap(int);
int read_bitmap_file(const char *, t_bitmap *);
int read_bitmap_file_header(t_bitmap *, FILE *);
int read_bitmap_info_header(t_bitmap *, FILE *);
int read_bitmap_data(t_bitmap *, FILE *);
void streamout_bitmap(t_bitmap *);

#endif

#ifndef STEGATZYLIB_H
#define STEGATZYLIB_H

#define ENC_TYPE_PAD           99
#define ENC_TYPE_LSB           98
#define COVER_TYPE_BMP         97
#define COVER_TYPE_WAV         96

/* bmp functions */
size_t stegatzy_bmp_by_padding(FILE *, const char *);
size_t stegatzy_bmp_decode_padding(FILE *);
size_t stegatzy_bmp_by_lsb(FILE *, const char *);
size_t stegatzy_bmp_decode_lsb(FILE *);
size_t get_bmp_encode_size(t_bitmap *, char);
void stream_bmp_padding_contents(FILE *, t_bitmap *, size_t, size_t);
/* end of bmp functions */


/* wav functions */
size_t stegatzy_wav_by_lsb(FILE *, const char *, const char *);
size_t stegatzy_wav_decode_lsb(FILE *);
/* end of wav functions */


/* common functions */
int set_bit(byte *, uint8_t, uint8_t);
int set_bit16(uint16_t *, uint8_t, uint8_t);
int set_bit32(uint32_t *, uint8_t, uint8_t);
uint8_t get_bit(byte *, uint8_t);
void str_to_binary(const char *, uint8_t **);
void char_to_binary(char, uint8_t *);
void print_signed_hex(int16_t);
/* end of common functions */

#endif

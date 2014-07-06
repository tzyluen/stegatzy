#ifndef STEGATZYLIB_H
#define STEGATZYLIB_H

#define ENC_TYPE_PAD           99
#define ENC_TYPE_LSB           98

size_t stegatzy_by_padding(FILE *, const char *);
size_t stegatzy_decode_padding(FILE *);
size_t stegatzy_by_lsb(FILE *, const char *);
size_t stegatzy_decode_lsb(FILE *);
size_t get_encode_size(t_bitmap *, char);
int set_bit(byte *, uint8_t, uint8_t);
uint8_t get_bit(byte *, uint8_t);
void stream_padding_contents(FILE *, t_bitmap *, size_t, size_t);
void str_to_binary(const char *, uint8_t **);
void char_to_binary(char, uint8_t *);

#endif

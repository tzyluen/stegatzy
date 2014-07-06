#ifndef STEGATZYLIB_H
#define STEGATZYLIB_H

#define ENC_TYPE_PAD           99
#define ENC_TYPE_LSB           98

size_t stegatzy_by_padding(FILE *, const char *);
size_t stegatzy_by_lsb(FILE *, const char *);
size_t get_encode_size(t_bitmap *, char);
void stream_padding_contents(FILE *, t_bitmap *, size_t, size_t);

#endif

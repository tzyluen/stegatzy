#ifndef AES_H_
#define AES_H_

unsigned char *rand_IV(uint8_t);
size_t encrypt(void**, int, unsigned char*, unsigned char*, char*);
size_t decrypt(void**, int, unsigned char*, unsigned char*, char*);
void display(char*, int);

#endif

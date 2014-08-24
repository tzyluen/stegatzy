#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <mcrypt.h>
#include "aes.h"

/**
 * Random Initialization Vector generator:
 */
unsigned char *rand_IV(uint8_t length)
{
    unsigned char *iv_str = malloc((sizeof(char) * length) + 1);
    srand((uint8_t) time(0) + getpid());
    int i;
    for (i = 0; i < length; ++i) {
        iv_str[i] = rand() % 19 + 83;
        srand(rand());
    }
    iv_str[i] = '\0';

    return iv_str;
}


size_t encrypt(void **buf,
            int buf_len,
            unsigned char *IV,
            unsigned char *key,
            char *enc)
{
    MCRYPT td = mcrypt_module_open(enc, NULL, "cbc", NULL);
    if (td == MCRYPT_FAILED)
        return 1;
    int blocksize = mcrypt_enc_get_block_size(td);
    int n_blocks = buf_len / blocksize;
    int key_len = mcrypt_enc_get_iv_size(td);
    //printf("(%d) iv_size: %d\n", __LINE__, mcrypt_enc_get_iv_size(td));
    unsigned char *tempbuf = NULL;
    printf("(%d) n_blocks: %d\n", __LINE__, n_blocks);

    if (buf_len % blocksize != 0) {
        fprintf(stderr, "(%d) buf_len %% blocksize != 0: %d, blocksize: %d\n",
                __LINE__, buf_len % blocksize, blocksize);
        int extra_bytes = blocksize - (buf_len % blocksize);
        printf("attempt to fix by padding extra %d bytes\n", extra_bytes);
        int expanded_buf_len = buf_len + extra_bytes;
        printf("from %d to total %d bytes\n", buf_len, expanded_buf_len);
        //unsigned char *tempbuf = realloc(buf, expanded_buf_len);
        tempbuf = malloc(expanded_buf_len * sizeof(char) + 1);
        memset(tempbuf, 0x20, expanded_buf_len);    // 0x20 is space
        memcpy(tempbuf, *buf, buf_len);

        tempbuf = tempbuf + expanded_buf_len;
        *tempbuf = 0x00;    // 0x00 is NULL
        n_blocks = expanded_buf_len / blocksize;
        tempbuf = tempbuf - expanded_buf_len;
        *buf = tempbuf;

        n_blocks = expanded_buf_len / blocksize;
        printf("(%d) n_blocks: %d\n", __LINE__, n_blocks);
        buf_len = expanded_buf_len;
    }
    printf("(%d) buf:%zu, tempbuf:%zu\n", __LINE__, strlen(*buf), strlen(tempbuf));

    mcrypt_generic_init(td, key, key_len, IV);
    int i;
    for (i = 0; i < n_blocks; i++) {
        mcrypt_generic(td, ((unsigned char*)*buf) + (i * blocksize), blocksize);
    }
    mcrypt_generic_deinit(td);
    mcrypt_module_close(td);

    return buf_len;
}


size_t decrypt(void **buf,
            int buf_len,
            unsigned char *IV,
            unsigned char *key,
            char *enc)
{
    MCRYPT td = mcrypt_module_open(enc, NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);
    int n_blocks = buf_len / blocksize;
    int key_len = mcrypt_enc_get_iv_size(td);

    /*if (buf_len % blocksize != 0) {
        fprintf(stderr, "(%d) buf_len %% blocksize != 0: %d, blocksize: %d\n",
                __LINE__, buf_len % blocksize, blocksize);
        return 1;
    }*/

    mcrypt_generic_init(td, key, key_len, IV);
    int i; 
    for (i = 0; i < n_blocks; ++i) {
        mdecrypt_generic(td, ((unsigned char*)*buf) + (i * blocksize), blocksize);
        //printf("(%d) '%s'\n", __LINE__, *buf);
    }
    mcrypt_generic_deinit(td);
    mcrypt_module_close(td);

    //printf("(%d) %zu\n", __LINE__, strlen(*buf));
    return strlen(*buf);
}


void display(char *ciphertext, int len)
{
    int v;
    for (v = 0; v < len; ++v) {
        if (v % 16 == 0)
            putchar('\n');
        printf("%02X", (unsigned char)ciphertext[v]);
    }
    printf("\n");
}

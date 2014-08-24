#define _BSD_SOURCE
#include <endian.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <mcrypt.h>
#include "aes.h"

#define ENC_DATA_SIZE        8
#define KEY_SIZE            16                  /* 128 bits */
#define IV_SIZE             16
#define EXEC_ENC_TYPE       "rijndael-128"      /* crypto algorithm */

static unsigned char KEY[16] = {
    0x53, 0x75, 0x7a, 0x79, 0x26, 0x52, 0x69, 0x63,
    0x68, 0x42, 0x65, 0x72, 0x67, 0x69, 0x6e, 0x20
};

int stitch(FILE *, FILE *);
int unstitch(FILE *, FILE *, bool);

typedef enum {
    STITCH,
    UNSTITCH
} prog_mode_t;


int main(int argc, char **argv)
{
    char *cover_file, *secret_file, *decrypted_file;
    cover_file = secret_file = decrypted_file = NULL;
    prog_mode_t mode;
    int index, c;
    bool truncate = false;
    mode = index = c = opterr = 0;
    FILE *dest_fp, *src_fp ;

    while ((c = getopt(argc, argv, "suti:x:c:")) != -1)
        switch (c) {
            case 's':
                mode = STITCH;
                break;
            case 'u':
                mode = UNSTITCH;
                break;
            case 'i':
                secret_file = optarg;
                break;
            case 't':
                truncate = true;
                break;
            case 'x':
                decrypted_file = optarg;
                break;
            case 'c':
                cover_file = optarg;
                break;
            case '?':
                if (optopt == 'i')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'x')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'c')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }

    if (mode == STITCH) {
        if (cover_file != NULL && secret_file != NULL) {
            src_fp = fopen(secret_file, "r");
            dest_fp = fopen(cover_file, "a");
            stitch(dest_fp, src_fp);
        } else
            abort();
    } else if (mode == UNSTITCH) {
        if (cover_file != NULL && decrypted_file != NULL) {
            src_fp = fopen(cover_file, truncate ? "r+" : "r");
            dest_fp = fopen(decrypted_file, "a");
            unstitch(dest_fp, src_fp, truncate);
        } else
            abort();
    } else
        abort();
    
    //printf("mode:%d, -i:%s, -o:%s\n", mode, infile, outfile);
    //for (index = optind; index < argc; index++)
    //    printf ("Non-option argument %s\n", argv[index]);
    fclose(dest_fp);
    fclose(src_fp);

    return EXIT_SUCCESS;
}


int stitch(FILE *dest_fp, FILE *src_fp)
{
    size_t length;
    fseek(src_fp, 0, SEEK_END);
    length = ftell(src_fp);

    fseek(src_fp, 0, SEEK_SET);
    unsigned char *buf = calloc(length, sizeof(char));
    printf("(%d) src_fp's length: %ld\n\n", __LINE__, length);

    if (buf) {
        if (fread(buf, 1, length, src_fp) != length) {
            printf("(%d) fread error\n", __LINE__);
            return -ENOMEM;
        }
    } else {
        printf("(%d) malloc error\n", __LINE__);
        return -ENOMEM;
    }

    MCRYPT td = mcrypt_module_open(EXEC_ENC_TYPE, NULL, "cbc", NULL);
    if (td == MCRYPT_FAILED)
        return 1;
    unsigned char *IV = rand_IV(mcrypt_enc_get_iv_size(td));
    printf("(%d) (%s)\n", __LINE__, IV);

    size_t enc_len = encrypt(&buf, length, IV, KEY, EXEC_ENC_TYPE);
    printf("(%d) %zu\n", __LINE__, enc_len);

    fwrite(buf, 1, enc_len, dest_fp);
    fwrite(&enc_len, 1, sizeof(size_t), dest_fp);
    fwrite(IV, 1, strlen(IV), dest_fp);

    mcrypt_generic_deinit(td);
    mcrypt_module_close(td);

    return EXIT_SUCCESS;
}


/**
 * tampered executable binary file format structure:
 *
 * |--------------| EOF |--------------| |---<int 4 Bytes>---| |16Bytes|
 * [Coverfile Data]  \0 [Encrypted Data] [Encrypted Data Size] [IV Keys]
 */
int unstitch(FILE *dest_fp, FILE *src_fp, bool truncate)
{
    size_t length, cover_file_off_t;
    unsigned char *buf = NULL;
    unsigned char *IV = malloc(IV_SIZE + 1);
    MCRYPT td = NULL;
    length = cover_file_off_t = 0;

    fseek(src_fp, -IV_SIZE -ENC_DATA_SIZE, SEEK_END);

    printf("(%d) %d\n", __LINE__, -IV_SIZE-ENC_DATA_SIZE);

    /* 1) get the encrypted size */
    fread(&length, 1, sizeof(size_t), src_fp);
    printf("(%d) %zu, 0x%x\n", __LINE__, length, length);

    /* 2) get the IV Key */
    fread(IV, 1, IV_SIZE, src_fp);
    IV = IV + IV_SIZE;
    *IV = '\0';
    IV = IV - IV_SIZE;
    printf("(%d) (%s)\n", __LINE__, IV);

    /* 3) get the encrypted data */
    fseek(src_fp, -IV_SIZE -ENC_DATA_SIZE -length, SEEK_END);

    cover_file_off_t = ftell(src_fp);   // get the cover file secret data offset

    printf("(%d) %zu\n", __LINE__, length);
    buf = calloc(length, sizeof(char));
    printf("(%d) src_fp's length: %ld\n\n", __LINE__, length);

    if (buf) {
        if (fread(buf, 1, length, src_fp) != length) {
            printf("(%d) fread error\n", __LINE__);
            return -ENOMEM;
        }
    } else {
        printf("(%d) malloc error\n", __LINE__);
        return -ENOMEM;
    }

    if (truncate) {
        int src_fd = fileno(src_fp);
        printf("(%d) %zu\n", __LINE__, cover_file_off_t);
        printf("(%d) %d\n", __LINE__, src_fd);
        if (ftruncate(src_fd, cover_file_off_t)) {
            fprintf(stderr, "(%d) errno: %d, %s\n", __LINE__, errno, strerror(errno));
        }
    }

    td = mcrypt_module_open(EXEC_ENC_TYPE, NULL, "cbc", NULL);
    if (td == MCRYPT_FAILED)
        return 1;

    if (decrypt(&buf, length, IV, KEY, EXEC_ENC_TYPE) != length)
        return EXIT_FAILURE;

    fwrite(buf, 1, length, dest_fp);
    mcrypt_generic_deinit(td);
    mcrypt_module_close(td);

    return EXIT_SUCCESS;
}

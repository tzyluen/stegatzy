Stegatzy
========
Least significant bit steganography implementation.

* Hide data in Bitmap image file
* Hide data in Waveform Audio File Format (WAV)
* Encrypt data with AES, and hide the encrypted data in ELF binary/executable file

Compile and run:
---------------

  ``` 
  $ make
  ```
  
Bitmap cover file
  ``` 
  $ ./stegatzy encode <cover-file> -t bmp -e lsb "secret text"
  $ ./stegatzy decode <cover-file> -t bmp -e lsb 
  ```
  
Wav cover file
  ``` 
  $ ./stegatzy encode <cover-file> -t wav -e lsb "secret text"
  $ ./stegatzy decode <cover-file> -t wav -e lsb 
  ```

Binary (ELF) cover file and AES encryption
  ``` 
  $ cd encryptbin; make
  $ ./hideinbin -s -c assets/xeyes -i secret.short
  $ ./hideinbin -u -c assets/xeyes -t -x decrypted.text
  ```

Demo
----
### Bitmat file format

1. Hide `"secret text"` into a bitmap image file:

    ![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-encode-bmp-lsb1.png)

2. Decode the LSB-encoded `"secret text"` from the bitmap image file:

    ![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-decode-bmp-lsb1.png)

Original image file:                   | Encoded image file:
:-------------------------------------:|:-------------------------------------:
![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/brucelee.before.bmp)  | ![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/brucelee.after.bmp)


### Wav file format

1. Encode and decode:

    ![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-encode-decode-wav-lsb1.png)

2. Before and after wav files waveform:

    ![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-wav-lsb-before-after.png)


### Encrypt data with AES, and hide the encrypted data in ELF binary/executable file

1. Encrypt and attach to encrypted data and keys to the cover file `xeyes` file:

    ```
    $ ./hideinbin -s -c assets/xeyes -i secret.short 
    (115) src_fp's length: 12

    (131) (bbWdT^TZSVZcW`ZU)
    (43) n_blocks: 0
    (47) buf_len % blocksize != 0: 12, blocksize: 16
    attempt to fix by padding extra 4 bytes
    from 12 to total 16 bytes
    (64) n_blocks: 1
    (67) buf:16, tempbuf:16
    (134) 16
    ```

2. Decrypt `secret text` from the cover file:

    ```
    $ ./hideinbin -u -c assets/xeyes -t -x decrypted.text
    (163) -24 
    (167) 16, 0x10
    (174) (bbWdT^TZSVZcW`ZU)
    (181) 16
    (183) src_fp's length: 16

    (197) 20944
    (198) 3
    ```


The tampered binary file format structure:
```
|--------------| EOF |--------------| |--<size_t Bytes>---| |16Bytes|
[Coverfile data]  \0 [Encrypted data] [Encrypted data size] [IV keys]
```

Hexdump of the binaries, before and after:

Original `xeyes` binary file           | Tampered `xeyes` binary file
:-------------------------------------:|:-------------------------------------:
![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-encryptbin-before.png)  | ![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-encryptbin-after.png)

* Orange - Encrypted data
* Blue - `size_t` contains the encrypted data size
* Red - IV keys


References
----------
1. http://en.wikipedia.org/wiki/BMP_file_format
2. http://paulbourke.net/dataformats/bitmaps
3. http://en.wikipedia.org/wiki/Least_significant_bit
4. https://ccrma.stanford.edu/courses/422/projects/WaveFormat
5. http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html


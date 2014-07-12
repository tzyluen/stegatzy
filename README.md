Stegatzy
========
Steganography implementation.

Demo
----
Encode a bitmap image file
![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-encode-bmp-lsb1.png)

Decode a bitmap image file
![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-decode-bmp-lsb1.png)

Before decode image file:                                                           | After encoded image file:
:----------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------:
![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/brucelee.before.bmp)  | ![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/brucelee.after.bmp)

Encode and decode a wav file
![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-encode-decode-wav-lsb1.png)

Before and after wav files waveform:
![alt tag](https://raw.github.com/tzyluen/stegatzy/master/img/stegatzy-wav-lsb-before-after.png)



Reference
---------
1. http://en.wikipedia.org/wiki/BMP_file_format
2. http://paulbourke.net/dataformats/bitmaps
3. http://en.wikipedia.org/wiki/Least_significant_bit
4. https://ccrma.stanford.edu/courses/422/projects/WaveFormat
5. http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

Sample bmp files
----------------
1. _Bruce Lee_ (good medium, lot of image noise)
2. _IP Man_

Sample wav files
----------------
1. Martin Luther King Jr; _I have a Dream_. From https://archive.org/download/MLKDream/MLKDream.wav (**PCM**)
2. J.S. Bach; _Partita E Major, Gavotte en rondeau_. From http://www.music.helsinki.fi/tmt/opetus/uusmedia/esim/a2002011001-e02.wav (**Non-PCM**)
3. (**Microsoft Extensible Format**)

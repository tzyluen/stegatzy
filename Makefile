CXX=gcc
CXXFLAGS=-std=c99 -O0 -g -Wall
BITMAPFILE=hello.bmp
SECRETTEXT=Lisa P's back!
#WAVFILE=JSBach-Partita-E-major.wav
WAVFILE=MLKDream.wav

all:
	$(CXX) $(CXXFLAGS) tzybitmap.c stegatzylib.c tzywav.c -c
	$(CXX) $(CXXFLAGS) tzybitmap.o stegatzylib.o tzywav.o stegatzy.c -o stegatzy

clean:
	rm -f *o a.out stegatzy $(BITMAPFILE)
	rm -f new_*.wav

create_bmp:
	./stegatzy create $(BITMAPFILE) -c 24 401 401 255 255 255

encode_bmp_pad:
	./stegatzy encode $(BITMAPFILE) -t bmp -e pad "$(SECRETTEXT)"

encode_bmp_lsb:
	./stegatzy encode $(BITMAPFILE) -t bmp -e lsb "$(SECRETTEXT)"

decode_bmp_pad:
	./stegatzy decode $(BITMAPFILE) -t bmp -e pad

decode_bmp_lsb:
	./stegatzy decode $(BITMAPFILE) -t bmp -e lsb

encode_wav_lsb:
	./stegatzy encode $(WAVFILE) -t wav -e lsb "$(SECRETTEXT)"

decode_wav_lsb:
	./stegatzy decode new_$(WAVFILE) -t wav -e lsb

memcheck:
	valgrind ./stegatzy encode $(BITMAPFILE) -t pad "$(SECRETTEXT)"

leakcheck:
	valgrind --leak-check=full ./stegatzy encode $(BITMAPFILE) -t pad "$(SECRETTEXT)"

fullcheck:
	valgrind --leak-check=full --track-origin=yes ./stegatzy encode $(BITMAPFILE) -t pad "$(SECRETTEXT)"

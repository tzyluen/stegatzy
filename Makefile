CXX=gcc
CXXFLAGS=-std=c99 -O0 -g -Wall

all:
	$(CXX) $(CXXFLAGS) tzybitmap.c stegatzylib.c -c
	$(CXX) $(CXXFLAGS) tzybitmap.o stegatzylib.o stegatzy.c -o stegatzy

clean:
	rm -f *o a.out stegatzy hello.bmp

create:
	./stegatzy create hello.bmp -c 24 401 401 255 255 255

encode_pad:
	./stegatzy encode hello.bmp -t pad "Lisa P's back!"

encode_lsb:
	./stegatzy encode hello.bmp -t lsb "Lisa P's back!"

decode_pad:
	./stegatzy decode hello.bmp -t pad

decode_lsb:
	./stegatzy decode hello.bmp -t lsb

memcheck:
	valgrind ./stegatzy encode hello.bmp -t pad "untold secret"

leakcheck:
	valgrind --leak-check=full ./stegatzy encode hello.bmp -t pad "untold secret"

fullcheck:
	valgrind --leak-check=full --track-origin=yes ./stegatzy encode hello.bmp -t pad "untold secret"

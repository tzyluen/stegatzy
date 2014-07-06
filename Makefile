CXX=gcc
CXXFLAGS=-std=c99 -O0 -g -Wall

all:
	$(CXX) $(CXXFLAGS) tzybitmap.c stegatzylib.c -c
	$(CXX) $(CXXFLAGS) tzybitmap.o stegatzylib.o stegatzy.c -o stegatzy

clean:
	rm -f *o stegatzy hello.bmp

create:
	./stegatzy create hello.bmp -c 24 401 401 255 255 255

encode:
	./stegatzy encode hello.bmp -t pad "Lisa P's back!"

decode:
	./stegatzy decode hello.bmp -t pad

memcheck:
	valgrind ./stegatzy encode hello.bmp -t pad "untold secret"

leakcheck:
	valgrind --leak-check=full ./stegatzy encode hello.bmp -t pad "untold secret"

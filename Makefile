CXX=gcc
CXXFLAGS=-O2 -g -Wall

all:
	$(CXX) $(CXXFLAGS) tzybitmap.c -c
	$(CXX) $(CXXFLAGS) tzybitmap.o stegatzy.c -o stegatzy

clean:
	rm -f *o stegatzy

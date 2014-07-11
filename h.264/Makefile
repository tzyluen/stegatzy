CXX=gcc
CXXFLAGS=-std=c99 -O0 -g -Wall
H264FILE=sample_iPod.m4v
LIBS_ffmpeg=-lm -lz -lpthread -lavformat -lavcodec -lavutil

LIBS=$(LIBS_ffmpeg)

all:
	$(CXX) $(CXXFLAGS) $(LIBS) tzyh264.c -c
	$(CXX) $(CXXFLAGS) $(LIBS) tzyh264.o h264main.c -o h264main

clean:
	rm -f *.o a.out h264main

read:
	./h264main $(H264FILE)

.SUFFIXES : .c
.SUFFIXES : .cpp

INCDIR=
LIBDIR=
CPP=g++

SRCS= $(wildcard *.cpp)
OBJS= $(SRCS:.cpp=.o) 

CFLAGS= -Wall -O3 -ffast-math -flto -march=armv8-a+crypto -mcpu=cortex-a57+crypto $(shell pkg-config --cflags opencv) 
LDFLAGS= $(shell pkg-config --libs opencv) -lpthread

TARGET= main.out

$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LIBDIR) $(LDFLAGS) 

.c.o: 
	$(CPP) -c $(CFLAGS) $(INCDIR) $< 

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCDIR) $<

clean:
	rm -f $(OBJS) $(TARGET)

encode:
	ffmpeg -framerate 30 -pattern_type glob -i 'output_frames/*.jpg' -c:v libx264 -profile:v high -crf 20 -pix_fmt yuv420p out.mp4

purge: 
	make clean
	rm -f output_frames/*

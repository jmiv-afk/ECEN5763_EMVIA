.SUFFIXES : .c
.SUFFIXES : .cpp

INCDIR=
LIBDIR=
CPP=gcc

SRCS= $(wildcard *.c)
OBJS= $(SRCS:.c=.o) 

CFLAGS= -Wall -O3 -g -fopenmp
LDFLAGS= -fopenmp

TARGET= rgb_difference.out 

$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LIBDIR) $(LDFLAGS) 

.c.o: 
	$(CPP) -c $(CFLAGS) $(INCDIR) $< 

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCDIR) $<

clean:
	rm -f $(OBJS) $(TARGET)
	
run:
	mkdir -p PPM_out
	rm PPM_out/*
	./rgb_difference.out 
	make encode

encode:
	ffmpeg -framerate 29.97 -pattern_type glob -i 'PPM_out/*.ppm' -c:v libx264 -profile:v high -crf 20 -pix_fmt yuv420p rgb_difference.mp4

decode:
	echo "Generating folder and decoding with ffmpeg:"
	mkdir -p PPM_in
	ffmpeg -i ../Dark-Room-Laser-Spot-with-Clutter.mpeg PPM_in/DRLS%04d.ppm -threads 4

purge: 
	make clean
	echo "Purging:"
	rm -rfi PPM_out PPM_in rgb_difference.mp4


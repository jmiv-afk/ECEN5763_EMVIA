.SUFFIXES : .c
.SUFFIXES : .cpp

INCDIR=
LIBDIR=
CPP=gcc

SRCS= $(wildcard *.c)
OBJS= $(SRCS:.c=.o) 

CFLAGS= -Wall -O3 -g -fopenmp
LDFLAGS= -fopenmp

TARGET= lightroom.out

$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LIBDIR) $(LDFLAGS) 

.c.o: 
	$(CPP) -c $(CFLAGS) $(INCDIR) $< 

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCDIR) $<

clean:
	rm -f $(OBJS) $(TARGET)
	
run:
	make
	mkdir -p PPM_out
	rm -rf PPM_out/* lightroom.mp4
	./lightroom.out 
	make encode

encode:
	ffmpeg -framerate 29.97 -pattern_type glob -i 'PPM_out/*.ppm' -c:v libx264 -profile:v high -crf 20 -pix_fmt yuv420p lightroom.mp4

decode:
	echo "Generating folder and decoding with ffmpeg:"
	mkdir -p PPM_in
	ffmpeg -i ../Light-Room-Laser-Spot-with-Clutter.mpeg -s 960x540 PPM_in/DRLS%04d.ppm -threads 4

purge: 
	make clean
	echo "Purging:"
	rm -rfi PPM_out PPM_in lightroom.mp4
	

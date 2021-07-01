.SUFFIXES : .c
.SUFFIXES : .cpp

INCDIR=
LIBDIR=
CPP=gcc

SRCS= $(wildcard *.c)
OBJS= $(SRCS:.c=.o) 

CFLAGS= -Wall -O3 -g -fopenmp
LDFLAGS= -fopenmp

TARGET= graymap.out 

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
	mkdir -p PGM_out
	rm -rf PGM_out/*
	./graymap.out 
	make encode

encode:
	ffmpeg -framerate 29.97 -pattern_type glob -i 'PGM_out/*.pgm' -c:v libx264 -profile:v high -crf 20 -pix_fmt yuv420p graymap.mp4

decode:
	echo "Generating folder and decoding with ffmpeg:"
	mkdir -p PPM_in
	ffmpeg -i ../Dark-Room-Laser-Spot.mpeg -s 960x540 PPM_in/DRLS%04d.ppm -threads 4

purge: 
	make clean
	echo "Purging:"
	rm -rfi PGM_out PPM_in graymap.mp4


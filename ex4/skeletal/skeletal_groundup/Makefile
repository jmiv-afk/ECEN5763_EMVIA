.SUFFIXES : .c
.SUFFIXES : .cpp

INCDIR=
LIBDIR=
CPP=gcc

SRCS= $(wildcard *.c)
OBJS= $(SRCS:.c=.o) 

CFLAGS= -Wall -O3 -g -fopenmp
LDFLAGS= -fopenmp

TARGET= main.out

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
	rm -rf PGM_out/* skel_out.mp4
	./main.out 
	make encode

encode:
	ffmpeg -framerate 30 -pattern_type glob -i 'PGM_out/*.pgm' -c:v libx264 -profile:v high -crf 20 -pix_fmt yuv420p skel_out.mp4

create-video: 
	rm gestures.mp4
	ffmpeg -f v4l2 -framerate 30 -video_size 640x480 -i /dev/video0 gestures.mp4
	

decode:
	echo "Generating folder and decoding with ffmpeg:"
	mkdir -p PPM_in
	ffmpeg -i gestures.mp4 -s 640x480 PPM_in/%04d.ppm -threads 4

purge: 
	make clean
	echo "Purging:"
	rm -rf PGM_out PPM_in skel_out.mp4
	

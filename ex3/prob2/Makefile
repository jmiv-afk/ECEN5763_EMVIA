.SUFFIXES : .c
.SUFFIXES : .cpp

INCDIR=
LIBDIR=
CPP=gcc

SRCS= $(wildcard *.c)
OBJS= $(SRCS:.c=.o) 

CFLAGS= -Wall -O3 -g 
LDFLAGS= 

TARGET= median_filter.out

$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LIBDIR) $(LDFLAGS) 

.c.o: 
	$(CPP) -c $(CFLAGS) $(INCDIR) $< 

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCDIR) $<

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f DRLS001_nofilt.pgm DRLS001_filt.pgm
	
run:
	./median_filter.out DRLS001.ppm DRLS001_nofilt.pgm n
	./median_filter.out DRLS001.ppm DRLS001_filt.pgm y



INCLUDE_DIRS=
LIB_DIRS= 
CC=g++

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lrt
CPPLIBS= 

HFILES= 
CFILES= 

SRCS= ${HFILES} ${CFILES}

all:	capture stereo_match 

clean:
	-rm -f *.o *.d
	-rm -f capture
	-rm -f capture_stereo
	-rm -f stereo_match

distclean:
	-rm -f *.o *.d

capture_stereo: capture_stereo.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $@.o `pkg-config --libs opencv` $(CPPLIBS)

capture: capture.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $@.o `pkg-config --libs opencv` $(CPPLIBS)

stereo_match: stereo_match.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $@.o `pkg-config --libs opencv` $(CPPLIBS)

depend:

run:
	./stereo_match left1.jpg right1.jpg 

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<

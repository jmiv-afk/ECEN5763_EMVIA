.SUFFIXES : .c
.SUFFIXES : .cpp

INCDIR=
LIBDIR=
CPP=g++

SRCS= $(wildcard *.cpp)
OBJS= $(SRCS:.cpp=.o) 

CFLAGS= -Wall -O3 $(shell pkg-config --cflags opencv)
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

purge: 
	make clean
	rm -f output_video/*

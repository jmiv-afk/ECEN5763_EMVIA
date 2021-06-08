.SUFFIXES : .c
.SUFFIXES : .cpp

INCDIR=
LIBDIR=
CPP=g++

SRCS= $(wildcard *.cpp)
OBJS= $(SRCS:.cpp=.o)

CFLAGS= -Wall -O3 -g $(INCDIR) 
LDFLAGS= -lm -lopencv_core -lopencv_flann -lopencv_video -lrt  

TARGET= a.out

$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LIBDIR) $(LDFLAGS) 

.c.o: 
	$(CPP) -c $(CFLAGS) $(INCDIR) $< 

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCDIR) $<

clean:
	rm -f $(OBJS) $(TARGET)


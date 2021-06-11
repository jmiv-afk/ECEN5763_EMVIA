.SUFFIXES : .c
.SUFFIXES : .cpp

INCDIR=
LIBDIR=
CPP=g++

SRCS= $(wildcard *.cpp)
OBJS= $(SRCS:.cpp=.o)

CFLAGS= -Wall -O3 -g 
LDFLAGS= -lopencv_dnn -lopencv_ml -lopencv_objdetect -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_flann -lopencv_core

TARGET= my_opencv 

$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LIBDIR) $(LDFLAGS) 

.c.o: 
	$(CPP) -c $(CFLAGS) $(INCDIR) $< 

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCDIR) $<

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET) 
	./my_opencv --cascade="haarcascade_frontalface_alt.xml" --nested-cascade="haarcascade_eye.xml" lena.jpg

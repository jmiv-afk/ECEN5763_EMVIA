/*
 *
 *  Example by Sam Siewert 
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


int main( int argc, char** argv ) {
  
  Mat frame;
  char filename[10];
  
  cvNamedWindow("Frame", CV_WINDOW_AUTOSIZE);

  VideoCapture cap(0);
  if (!cap.isOpened()) {
    cout << "cap(0) unable to be opened, aborting\n";
    return -1;
  }


  printf("width: %f\n", cap.get(cv::CAP_PROP_FRAME_WIDTH));
  printf("height: %f\n", cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);


  int i = 0;

  while(1) {

    cap >> frame;
    if( frame.empty() ) break;

    imshow("Frame", frame);

    char keypress = (char) waitKey(25); 

    if( keypress == 's') {
      sprintf(filename, "%0d.ppm", i++);
      imwrite(filename, frame);
    }
  
    if( keypress == 'q') {
      return -1; 
    }
  }
  
  return 0;
}

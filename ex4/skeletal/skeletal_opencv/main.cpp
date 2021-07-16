/* ----------------------------------------------------------------------------
 * @file main.cpp
 * @brief Continuous webcam skeletal transform using OpenCV 
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 * @resources Adapted to OpenCV3 from Sam Siewert's skeletal examples
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include "log.h"

using namespace cv;
using namespace std; 

//#define BACKGROUND_ELIM

// the main program
int main(int argc, char **argv) {

  Mat frame, gray, binary, mfblur;

  cvNamedWindow("frame", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("binary", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("skel", CV_WINDOW_AUTOSIZE);

#ifdef BACKGROUND_ELIM
  // create Background Subtractor objects
  Ptr<BackgroundSubtractor> pBackSub;
  pBackSub = createBackgroundSubtractorMOG2(100, 16, false);
  //pBackSub = createBackgroundSubtractorKNN(200, 400.0, false);

  VideoWriter video_out(
    "output.mp4",
    CV_FOURCC('m','p','4','v'),
    3.4, // experimentally determined
    Size(640, 480),
    false
  );
#else

  VideoWriter video_out(
    "output.mp4",
    CV_FOURCC('m','p','4','v'),
    1.2, // experimentally determined
    Size(640, 480),
    false
  );

#endif

  // open default interface /dev/video0
  VideoCapture cap(0);
  if (!cap.isOpened()) {
    cout << "cap(0) unable to be opened, aborting\n";
    return -1;
  }

  double prev = get_time_msec();
  double start = prev;
  int framecnt = 0;

  while(1) {

    cap >> frame;
    if( frame.empty() ) break;

#ifndef BACKGROUND_ELIM
    // old code from Siewert:
    cvtColor(frame, gray, CV_BGR2GRAY);
    // binary threshold
    threshold(gray, binary, 127, 255, CV_THRESH_BINARY);
    medianBlur(binary, mfblur, 7);
#else 
    // Proposed new code using background elimination:
    pBackSub->apply(frame, binary);
    medianBlur(binary, mfblur, 7);
#endif 

    // This section of code was adapted from the following post, which was
    // based in turn on the Wikipedia description of a morphological skeleton
    //
    // http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/
    //
    Mat skel(mfblur.size(), CV_8UC1, Scalar(0));
    Mat temp;
    Mat eroded;
    Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
    bool done;
    int iterations=0;

    do
    {
      erode(mfblur, eroded, element);
      dilate(eroded, temp, element);
      subtract(mfblur, temp, temp);
      bitwise_or(skel, temp, skel);
      eroded.copyTo(mfblur);

      done = (countNonZero(mfblur) == 0);
      iterations++;
    
    } while (!done && (iterations < 100));

    //imshow("frame", frame);
    imshow("binary", binary);
    imshow("skel", skel);

    video_out << skel;
    //LOGSYS("%u, %6.2f", framecnt, get_time_msec()-prev);
    prev = get_time_msec();
    framecnt++;
    
    if ( waitKey(10) == 'q' ) break;
  }

  LOGP("FPS: %6.3f\n", framecnt*1000/(prev-start) );

  return 0;
}

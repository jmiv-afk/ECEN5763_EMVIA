/* ----------------------------------------------------------------------------
 * @file main.cpp
 * @brief
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 * @resources
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include "log.h"
#include "lane.h"

//#define INPUT_VIDEO ("input_video/02260001.AVI")
#define INPUT_VIDEO ("input_video/22400001.AVI")

using namespace cv;
using namespace std;


// the main program
int main(int argc, char **argv) {

  double prev, start;
  unsigned int framecnt = 0;
  Mat temp, raw, gray, gray_detect, pyr, blurred, thresh, hls;
  Rect roibox;

  //cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("ROI-Binary", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("raw", CV_WINDOW_AUTOSIZE);
  //cvNamedWindow("gray", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("gray_detect", CV_WINDOW_AUTOSIZE);

  VideoCapture cap(INPUT_VIDEO);
  cap.set(CAP_PROP_POS_MSEC, 430000);

  VideoWriter video_out(
    "output.mp4",
    CV_FOURCC('m','p','4','v'),
    cap.get(CAP_PROP_FPS), // experimentally determined
    Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)),
    false
  );

  start = get_time_msec();

  while(1) {

    cap >> raw;
    if( raw.empty() ) break;

    cvtColor(raw, gray, COLOR_BGR2GRAY);

    lane_detect(gray, &gray_detect, &roibox);
    pyrDown(gray, pyr);
    pyrDown(pyr, pyr);

    rectangle(raw, roibox, Scalar(0, 0, 255));

    //imshow("pyr", pyr);
    imshow("gray_detect", gray_detect);
    imshow("raw", raw);

    prev = get_time_msec();
    framecnt++;
    
    char user_input = waitKey(10);
    if ( user_input == 'q' ) break;
    //while( user_input != 'n' ) { user_input = waitKey(10); }
  }

  LOGP("FPS: %6.3f\n", framecnt*1000/(prev-start) );
  imwrite("output.png", pyr);

  return 0;
}

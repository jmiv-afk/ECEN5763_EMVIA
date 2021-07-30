/* ----------------------------------------------------------------------------
 * @file main.cpp
 * @brief A prototype self-driving assistant feature for lane detection and 
 *        departure warnings
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 * @resources 
 *  - Learning OpenCV3, Kaehler, Bradski
 *  - Sam Siewerts example codes
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/utility.hpp>

#include "log.h"
#include "lane.h"

using namespace cv;
using namespace std;

// the keys for the command line arguments
const String parser_keys =
  "{help h usage ? | | Print help message. }"
  "{input i  | input_video/clip1.avi      | Full filepath to input video.  }"
  "{output o | output_video/clip1_out.avi | Full filepath to output video. }"
  "{show     | 1 | Shows intermediate image pipeline steps. }"
  ;

// global variables extracted from the parser - application settings
String input_video_g;
String output_video_g;

//
// the main program
//
int main(int argc, char **argv) {

  bool show_pipeline;

  // 
  // command line parsing for application settings  
  //
  CommandLineParser parser(argc, argv, parser_keys);

  if (parser.has("help")) {
    parser.printMessage();
    return 0;
  }

  input_video_g = parser.get<String>("input");
  output_video_g = parser.get<String>("output");
  show_pipeline = parser.get<bool>("show");
  

  if (show_pipeline) {
    cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("2", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("3", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("4", CV_WINDOW_AUTOSIZE);
  }

  VideoCapture cap(input_video_g);
  cap.set(CAP_PROP_POS_MSEC, 10000);
  
  VideoWriter video_out(
    output_video_g,
    CV_FOURCC('M','J','P','G'),
    cap.get(CAP_PROP_FPS), // experimentally determined
    Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)),
    false
  );

  //
  // algorithm begin  
  //
  unsigned int framecnt = 0;
  double start = get_time_msec();
  double prev = get_time_msec();
  Mat raw;
  LaneDetector detector(show_pipeline);

  while(1) {

    cap >> raw;
    if( raw.empty() ) break;

    detector.input_image(raw);
    detector.detect();
    detector.annotate();

    prev = get_time_msec();
    framecnt++;
    
    char user_input = waitKey(10);
    if ( user_input == 'q' ) break;
    //while( user_input != 'n' ) { user_input = waitKey(10); }
    if (prev-start > 10*MSEC_TO_SEC) {break;}
  }

  LOGP("FPS: %6.3f\n", framecnt*1000/(prev-start) );

  return 0;
}

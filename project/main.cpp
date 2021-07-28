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
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include "log.h"
#include "lane.h"

using namespace cv;
using namespace std;

// the keys for the command line arguments
const String parser_keys =
  "{help h usage ? | | Print help message. }"
  "{input i  | input_video/clip1.mp4      | Full filepath to input video.  }"
  "{output o | output_video/clip1_out.mp4 | Full filepath to output video. }"
  "{show     | 0 | Shows intermediate image pipeline steps. }"
  ;

// global variables extracted from the parser - application settings
String input_video_g;
String output_video_g;
bool show_pipeline_g;

//
// the main program
//
int main(int argc, char **argv) {

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
  show_pipeline_g = parser.get<bool>("show");
  
  cout << input_video_g << endl;
  cout << output_video_g << endl;
  cout << show_pipeline_g << endl;
  
  //
  // algorithm begin  
  //
  double prev, start;
  unsigned int framecnt = 0;
  Mat temp, raw, gray, gray_detect, pyr, blurred, thresh, hls;
  Rect roibox;

  if (show_pipeline_g) {
    cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("ROI-Binary", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("raw", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("gray", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("gray_detect", CV_WINDOW_AUTOSIZE);
  }

  VideoCapture cap(input_video_g);

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

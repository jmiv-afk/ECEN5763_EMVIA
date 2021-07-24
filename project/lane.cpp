/* ----------------------------------------------------------------------------
 * @file lane.cpp
 * @brief
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 * @resources
 *---------------------------------------------------------------------------*/

#include "log.h"
#include "lane.h"

using namespace cv;

// The following moving average filter was sourced from:
// https://tttapa.github.io/Pages/Mathematics/Systems-and-Control-Theory/Digital-filters/Simple%20Moving%20Average/C++Implementation.html

template <uint8_t N, class input_t = uint16_t, class sum_t = uint32_t>
class SMA {
  public:
    input_t operator()(input_t input) {
        sum -= previousInputs[index];
        sum += input;
        previousInputs[index] = input;
        if (++index == N)
            index = 0;
        return (sum + (N / 2)) / N;
    }

    static_assert(
        sum_t(0) < sum_t(-1),  // Check that `sum_t` is an unsigned type
        "Error: sum data type should be an unsigned integer, otherwise, "
        "the rounding operation in the return statement is invalid.");

  private:
    uint8_t index             = 0;
    input_t previousInputs[N] = {};
    sum_t sum                 = 0;
};

void lane_detect(Mat img, Mat* ret, Rect* roibox) {
  
  Mat temp, roi;
  static SMA<10> filter;

  // crop the region of interest
  crop_ROI(img, &roi, roibox);
  
  // gaussian blur
  blur(roi, roi, Size(3,3));

  // determine threshold using Otsu's adaptive threshold, slightly raise
  int thresh = filter(threshold(roi, temp, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU) + 10);
  threshold(roi, temp, thresh, 255, CV_THRESH_BINARY);

  //imshow("ROI-Binary", temp);

  // Begin Hough transform algorithm
  std::vector<Vec4i> lines;
  Vec4i left, right;
  int i = 0;

  // detect the lines - from opencv tutorial
  HoughLinesP(
      temp,          // image
      lines,         // lines
      1,             // rho resolution of accumulator in pixels
      //CV_PI/180,   // theta resolution of accumulator 
      CV_PI/360,     // theta resolution of accumulator in pixels
      20,            // accumulator threshold, only lines >threshold returned
      80,            // minimum line length, lines shorter are rejected
      100            // maximum line gap between points on the same line for linking
  );


  // don't do any detection if no lines are found
  if (lines.size() == 0) {
    *ret = temp;
    return;
  }

  bool is_left_found = false;
  bool is_right_found = false;

  while( !(is_left_found && is_right_found) ) {

    Vec4i l = lines[i];

    float angle = atan2( l[1]-l[3], l[0]-l[2]);
    //float angle = atan( (l[1]-l[3])/(l[0]-l[2]) );

    //std::cout << "0: " << l[0] << std::endl;
    //std::cout << "1: " << l[1] << std::endl;
    //std::cout << "2: " << l[2] << std::endl;
    //std::cout << "3: " << l[3] << std::endl;
    //std::cout << "theta: " << angle << std::endl;

    // wrap to betw. 0 and PI
    if (angle < 0) { angle += CV_PI; }
    
    // perform some left/right filtering based on angle 
    if (angle >= 1.57 && angle < 2.53 && !is_left_found ) {
      left = lines[i];
      is_left_found = true;
      //std::cout << "left: " <<  angle << std::endl;
    }
    if (angle > 0.611 && angle < 1.57 && !is_right_found ) {
      right = lines[i];
      is_right_found = true;
      //std::cout << "right: " <<  angle << std::endl;
    }
    
    if ( !(i++ < lines.size()) ) {break;}
  
  }

  if (is_left_found) {
    line( roi, Point(left[0], left[1]), Point(left[2], left[3]), Scalar(0,0,255), 3, LINE_AA);
  }

  if (is_right_found) {
    line( roi, Point(right[0], right[1]), Point(right[2], right[3]), Scalar(0,0,255), 3, LINE_AA);
  }
  

  *ret = roi;
}

void crop_ROI(Mat img, Mat* ret, Rect* roibox) {

  Point topleft = Point(350, 430);
  *roibox = Rect(topleft, Size(380, 140));
  *ret = img(*roibox);
}

/*
void lane_mask_rgb(Mat hls_img, Mat* ret) {
  
  Mat white, yellow, temp1, temp2;
  
  Mat mask = Mat::zeros(hls_img.rows, hls_img.cols, CV_8U);

  //cout << "hres: " << hls_img.cols << endl;
  //cout << "vres: " << hls_img.rows << endl;

  Point pts[4] = {
    Point(280, 205),
    Point(360, 205),
    Point(451, 280),
    Point(168, 280)
  };
  fillConvexPoly( mask, pts, 4, cv::Scalar(255) );

  inRange(hls_img, Scalar(0, 90, 0), Scalar(255, 255, 255), white);
  inRange(hls_img, Scalar(10, 0, 30), Scalar(80, 255, 255), yellow);
  
  cuda::bitwise_and(mask, white, temp1);
  cuda::bitwise_and(mask, yellow, temp2);
  cuda::bitwise_or(temp1, temp2, *ret);

  //imshow("test", mask);
  //while(waitKey(20) != 'q') {}
}
*/

/* ----------------------------------------------------------------------------
 * @file lane.cpp
 * @brief Lane line detection and annotation class definitions
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 * @resources Learning OpenCV3 Tutorials
 *---------------------------------------------------------------------------*/

#include "lane.h"

/* @brief The default lane detector constructor
 *
 * assumes 1280x720 BGR color input images, and sets a pre-defined ROI 
 */
LaneDetector::LaneDetector() {
  
  // set default ROI
  // . . . . . . . . . . . . 
  // . . (0) . . . . (1) . . 
  // . . . + . . . . + . . . 
  // . . . . . . . . . . . . 
  // . . . + . . . . + . . .
  // . . (3) . . . . (2) . .
  // . . . . . . . . . . . . 
  roi_pts[0] = Point(350, 430); // top left
  roi_pts[1] = Point(750, 430); // top right
  roi_pts[2] = Point(750, 567); // bottom right
  roi_pts[3] = Point(350, 567); // bottom left
  //roi_pts[0] = Point(350, 430); // top left
  //roi_pts[1] = Point(830, 430); // top right
  //roi_pts[2] = Point(830, 567); // bottom right
  //roi_pts[3] = Point(350, 567); // bottom left

  proc_min = DBL_MAX;
  proc_max = 0.0;
  frame_num = 0;
  lines_detected = 0;
  is_left_found = false;
  is_right_found = false;
  vcenter = 605; // approximate vertical center 
}

void LaneDetector::show() {
  
  imshow("1", roi);
  imshow("2", annot);
}

/* @brief Detects left and right lane lines
 *
 * Upon completion, the Points left_pt<i>, and right_pt<i> will be present
 * denoting the location of the left and right lane lines in the raw frame.
 * Also, if no lane lines were detected, the is_left_found and is_right_found
 * booleans will be set. Frames can be annotated after detection.
 *
 * @param None
 * @return None
 */
void LaneDetector::detect() {

  cvtColor(*raw, gray, COLOR_BGR2GRAY);
  
  // crop the region of interest - just a rectangular region for now
  roi = gray(Rect(roi_pts[0], roi_pts[2]));

  // apply median filter
  medianBlur(roi, roi, 5);

  // use 5x5 mean adaptive threshold over binary image, slightly raise
  adaptiveThreshold(roi, roi, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 5, -2);

  // Begin Hough transform algorithm
  Vec4i left, right;

  // run the Hough transform
  hough_transform(left, right);

  if (is_left_found) {

    Point2f ret;
    Point2f pt1 = Point2f(left[0], left[1]);
    Point2f pt2 = Point2f(left[2], left[3]);

    //
    // check ROI top side intersection with lane line
    //
    if(intersection(Point(0,0), Point(1, 0), pt1, pt2, ret)) {
      left_pt1 = Point(round(ret.x), round(ret.y)) + roi_pts[0];
    } else {
      is_left_found = false;
    }

    //
    // check ROI bottom side intersection with lane line
    //
    if (intersection(Point(0,roi.rows-1), Point(1,roi.rows-1), pt1, pt2, ret)) {
      left_pt2 = Point(round(ret.x), round(ret.y)) + roi_pts[0];
    } else {
      is_left_found = false;
    }

  }

  if (is_right_found) {
    
    Point2f ret;
    Point2f pt1 = Point2f(right[0], right[1]);
    Point2f pt2 = Point2f(right[2], right[3]);

    //
    // check ROI top side intersection with lane line
    //
    if (intersection(Point(0,0), Point(1,0), pt1, pt2, ret)) {
      right_pt1 = Point(ret) + roi_pts[0];
    } else {
      is_right_found = false;
    }

    //
    // check ROI bottom side intersection with lane line
    //
    if (intersection(Point(0,roi.rows-1), Point(1,roi.rows-1), pt1, pt2, ret)) {
      right_pt2 = Point(ret) + roi_pts[0];
    } else {
      is_right_found = false;
    }
  }


} // end detect()


/* @brief Uses a standard hough transform to return coordinates of 
 *        left/right lane lines
 *
 * Operates on the binary ROI image and uses certain bounds on rho and 
 * theta for detecting left and right lane lines.
 *
 * @param left&, reference for the left lane line points - vectorized
 * @param right&, reference for the right lane line points - vectorized 
 *
 * @return None
 */
#define ACC_THRESH (30)
void LaneDetector::hough_transform(Vec4i& left, Vec4i& right) {

  std::vector<Vec3f> lines;
  unsigned int i=0;
 
  is_left_found = false;
  is_right_found = false;

  // detects left lane lines:
  HoughLines(
      roi,           // image
      lines,         // lines
      1,             // rho resolution of accumulator in pixels
      CV_PI/180,     // theta resolution of accumulator 
      ACC_THRESH,    // accumulator threshold, only lines >threshold returned
      0,             // srn - set to 0 for classical Hough
      0,             // stn - set to 0 for classical Hough
      0.174533,      // minimum theta 
      1.134464       // maximum theta 
  );

  
  while( !(is_left_found) && i < lines.size() ) {

    // sourced from OpenCV Hough tutorial:
    float rho = lines[i][0], theta = lines[i][1];
    if (abs(rho) > 90 && abs(rho) < 150) {
      //LOGP("rho: %f, theta: %f, votes: %f\n", rho, theta*180/CV_PI, lines[i][2]);
      double a = cos(theta), b = sin(theta);
      double x0 = a*rho, y0 = b*rho;
      left[0] = cvRound(x0 + 1000*(-b));
      left[1] = cvRound(y0 + 1000*(a));
      left[2] = cvRound(x0 - 1000*(-b));
      left[3] = cvRound(y0 - 1000*(a));
      is_left_found = true;
    }

    i++;
  }

  lines.clear(); i=0;

  // detects right lane lines:
  HoughLines(
      roi,           // image
      lines,         // lines
      1,             // rho resolution of accumulator in pixels
      CV_PI/180,     // theta resolution of accumulator 
      ACC_THRESH,    // accumulator threshold, only lines >threshold returned
      0,             // srn - set to 0 for classical Hough
      0,             // stn - set to 0 for classical Hough
      2.007129,      // minimum theta 
      2.967060       // maximum theta 
  );

  
  while( !(is_right_found) && i < lines.size() ) {

    // sourced from OpenCV Hough tutorial:
    float rho = lines[i][0], theta = lines[i][1];
    if ( abs(rho) > 150 && abs(rho) < 300) {
      //LOGP("rho: %f, theta: %f, votes: %f\n", rho, theta*180/CV_PI, lines[i][2]);
      double a = cos(theta), b = sin(theta);
      double x0 = a*rho, y0 = b*rho;
      right[0] = cvRound(x0 + 1000*(-b));
      right[1] = cvRound(y0 + 1000*(a));
      right[2] = cvRound(x0 - 1000*(-b));
      right[3] = cvRound(y0 - 1000*(a));
      is_right_found = true;
    }

    i++;
  }

}

/* @brief Checks if the point is valid within the bounds of annot
 */
bool LaneDetector::is_inside_annot(Point p) {

  if (0<=p.x && p.x<annot.cols && 0<=p.y && p.y<annot.rows) 
    return true;
  else
    return false;
}

/* @brief Finds the intersection of two lines, or returns false.
 * 
 * @param Points, the lines are defined by points (o1, p1) and (o2, p2).
 * @return r, the intersection point
 * 
 * @source 
 * https://stackoverflow.com/questions/7446126/opencv-2d-line-intersection-helper-function
 */
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r)
{
    Point2f x = o2 - o1;
    Point2f d1 = p1 - o1;
    Point2f d2 = p2 - o2;

    float cross = d1.x*d2.y - d1.y*d2.x;
    if (abs(cross) < /*EPS*/1e-8)
        return false;

    double t1 = (x.x * d2.y - x.y * d2.x)/cross;
    r = o1 + d1 * t1;
    return true;
}

#define RED    (Scalar( 96,  94, 211))
#define GREEN  (Scalar( 91, 186, 132))
#define BLUE   (Scalar(203, 147, 114))
#define YELLOW (Scalar( 61, 139, 148))
#define BLACK  (Scalar( 83,  81,  84))
/*
 * @brief Annotates a copy of the raw frame with lane lines
 */
void LaneDetector::annotate() {
  
  if (is_left_found 
      && is_inside_annot(left_pt1) 
      && is_inside_annot(left_pt2) ) {
    line(annot, left_pt1, left_pt2, RED, 3, LINE_4);
    lines_detected++;
  }

  if (is_right_found
      && is_inside_annot(right_pt1) 
      && is_inside_annot(right_pt2) ) {
    line(annot, right_pt1, right_pt2, RED, 3, LINE_4);
    lines_detected++;
  }

  // annotate ROI
  rectangle(annot, roi_pts[0], roi_pts[2], BLUE, 1, LINE_AA);  
  putText(annot, "ROI", roi_pts[0], FONT_HERSHEY_SIMPLEX, 0.5, BLUE, 1.5);

  unsigned int center_meas = (right_pt2.x + left_pt2.x)/2;
  int offset = center_meas - vcenter;
  Scalar tick_color;

  // annotate bottom black line
  line(annot, right_pt2, left_pt2, BLACK, LINE_8); 
  Point tick_bottom = Point(vcenter, right_pt2.y);
  Point center_meas_bottom = Point(center_meas, right_pt2.y); 
  if (abs(offset) > (right_pt2.x-left_pt2.x)/4) {
    tick_color = RED; 
    putText(annot, "!", roi_pts[1], FONT_HERSHEY_SIMPLEX, 0.5, RED, 1.5);
  } else if (abs(offset) > (right_pt2.x-left_pt2.x)/6) {
    tick_color = YELLOW;
  } else {
    tick_color = GREEN;
  }
  line(annot, tick_bottom+Point(0, -8), tick_bottom+Point(0, 8), tick_color, LINE_4); 
  
  if (is_left_found && is_right_found) {
    line(annot, center_meas_bottom+Point(0, -8), center_meas_bottom+Point(0, 8), RED, LINE_4); 
  }

  frame_num++;

  proc_end = get_time_msec();
  if (proc_end-proc_start < proc_min) {
    proc_min = proc_end-proc_start;
    LOGSYS("proc_min: %6.2f, frame: %i", proc_min, frame_num);
  }
  if (proc_end-proc_start > proc_max) {
    proc_max = proc_end-proc_start;
    LOGSYS("proc_max: %6.2f, frame: %i", proc_max, frame_num);
  }
  LOGSYS("MVPROC %6.2f", get_time_msec()); 
  proc_elapsed += proc_end-proc_start;

}


/*
 * @brief The raw image to use as input for the class
 */
void LaneDetector::input_image(Mat& img) {

  proc_start = get_time_msec();
  raw = &img;
  annot = Mat(*raw);
}

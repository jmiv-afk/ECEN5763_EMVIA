/* ----------------------------------------------------------------------------
 * @fileilane.cpp
 * @brief 
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 * @resources
 *---------------------------------------------------------------------------*/

#include "lane.h"

/* @brief the default lane detector constructor
 *
 * assumes 1280x720 color input images, and sets a pre-defined ROI 
 */
LaneDetector::LaneDetector(bool show_intermediate) {
  
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

  // setup ROI mask (black rectangle with white quadrilateral)
  //roi_mask = Mat::zeros(
  //    roi_pts[3].y-roi_pts[0].y, // delta y -- rows
  //    roi_pts[2].x-roi_pts[3].x, // delta x -- cols
  //    CV_8U // type
  //    );

  //int sniplen = 100;
  //assert(sniplen < roi_mask.cols && sniplen < roi_mask.rows);
  //Point pts[6];
  //pts[0] = Point(sniplen, 0); 
  //pts[1] = Point(roi_mask.cols-1 - sniplen, 0);
  //pts[2] = Point(roi_mask.cols-1, sniplen);
  //pts[3] = Point(0, sniplen); 
  
  //fillConvexPoly(roi_mask, pts, 4, cv::Scalar(255) );

  frame_num = 0;
  lines_detected = 0;
  is_left_found = false;
  is_right_found = false;
  show_pipeline = show_intermediate;
}

/* @brief
 *
 */
void LaneDetector::detect() {

  Mat temp;
  cvtColor(*raw, gray, COLOR_BGR2GRAY);
  
  // crop the region of interest - just a rectangular region for now
  roi = gray(Rect(roi_pts[0], roi_pts[2]));

  if (show_pipeline)
    imshow("1", roi);

  // determine threshold using Otsu's adaptive threshold, slightly raise
  //int thresh = threshold(roi, temp, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU) + 10;
  //threshold(roi, temp, thresh, 255, CV_THRESH_BINARY);
  
  medianBlur(roi, roi, 5);

  // use 3x3 mean adaptive threshold over binary image, slightly raise
  adaptiveThreshold(roi, roi, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 5, -2);

  // and in the mask to extract ROI
  //bitwise_and(temp, roi_mask, roi);

  if (show_pipeline)
    imshow("2", roi);

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
      // transform point into location in raw frame for annotation
      left_pt1 = Point(round(ret.x), round(ret.y)) + roi_pts[0];
    } else {
      is_left_found = false;
    }

    //
    // check ROI bottom side intersection with lane line
    //
    if (intersection(Point(0,roi.rows-1), Point(1,roi.rows-1), pt1, pt2, ret)) {
      // transform point into location in raw frame for annotation
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
      // transform point into location in raw frame for annotation
      right_pt1 = Point(ret) + roi_pts[0];
    } else {
      is_right_found = false;
    }

    //
    // check ROI bottom side intersection with lane line
    //
    if (intersection(Point(0,roi.rows-1), Point(1,roi.rows-1), pt1, pt2, ret)) {
      // transform point into location in raw frame for annotation
      right_pt2 = Point(ret) + roi_pts[0];
    } else {
      is_right_found = false;
    }
  }

} // end detect()

// run a Hough transform to get left/right points
// uses standard Hough
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
      CV_PI/90,      // theta resolution of accumulator 
      30,            // accumulator threshold, only lines >threshold returned
      0,             // srn - set to 0 for classical Hough
      0,             // stn - set to 0 for classical Hough
      0,             // minimum theta 
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
      CV_PI/90,      // theta resolution of accumulator 
      30,            // accumulator threshold, only lines >threshold returned
      0,             // srn - set to 0 for classical Hough
      0,             // stn - set to 0 for classical Hough
      2.007129,      // minimum theta 
      CV_PI          // maximum theta 
  );

  
  while( !(is_right_found) && i < lines.size() ) {

    // sourced from OpenCV Hough tutorial:
    float rho = lines[i][0], theta = lines[i][1];
    if ( abs(rho) > 230 && abs(rho) < 300) {
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

// run a Hough transform to get left/right points
// uses probabalistic Hough
void LaneDetector::hough_transform_P(Vec4i& left, Vec4i& right) {
  
  std::vector<Vec4i> lines;
  unsigned int i=0; 

  HoughLinesP(
      roi,           // image
      lines,         // lines
      1,             // rho resolution of accumulator in pixels
      CV_PI/180,     // theta resolution of accumulator 
      10,            // accumulator threshold, only lines >threshold returned
      20,            // minimum line length, lines shorter are rejected
      300            // maximum line gap between points on the same line for linking
  );

  is_left_found = false;
  is_right_found = false;

  while( !(is_left_found && is_right_found) ) {

    if (i == lines.size()-1) {break;}

    Vec4i l = lines[i];

    float angle = atan2( l[3]-l[1], l[2]-l[0] );

    if (angle < 0) {
      angle+=CV_PI;
    }

    // perform some left/right filtering based on angle 
    if (filter_by_angle(angle) == 'l' && !is_left_found) {
      left = lines[i];
      is_left_found = true;
    }
    
    if (filter_by_angle(angle) == 'r' && !is_right_found) {
      right = lines[i];
      is_right_found = true;
    }
    
    i++;
  }

}

// does angle filtering to characterize as left or right
char LaneDetector::filter_by_angle(float rad) {

  // Note that rad is measured from the negative x-axis due to OpenCV
  // column first notation and location of 0,0

  if ( (rad>1.588250 && rad<2.443461) || 
       (rad>4.729842 && rad<5.585054) ) {
    return 'l';
  }

  if ( (rad>0.698132 && rad<1.553343) || 
       (rad>3.839724 && rad>4.694936) ) {
    return 'r';
  }

  return 'n';
}


// checks if a point is validly inside the raw image
bool LaneDetector::is_inside_raw(Point p) {

  if (0<=p.x && p.x<raw->cols && 0<=p.y && p.y<raw->rows) 
    return true;
  else
    return false;
}

// Finds the intersection of two lines, or returns false.
// The lines are defined by (o1, p1) and (o2, p2).
//
// sourced from: 
// https://stackoverflow.com/questions/7446126/opencv-2d-line-intersection-helper-function
//
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

void LaneDetector::annotate() {
  
  Mat annot = Mat(*raw);

  if (is_left_found 
      && is_inside_raw(left_pt1) 
      && is_inside_raw(left_pt2) ) {
    line(annot, left_pt1, left_pt2, Scalar(0,0,255), 3, LINE_4);
    lines_detected++;
  }

  if (is_right_found
      && is_inside_raw(right_pt1) 
      && is_inside_raw(right_pt2) ) {
    line(annot, right_pt1, right_pt2, Scalar(0,0,255), 3, LINE_4);
    lines_detected++;
  }

  frame_num++;

  if (show_pipeline)
    imshow("3", annot);
}



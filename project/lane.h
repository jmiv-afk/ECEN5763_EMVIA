/* ----------------------------------------------------------------------------
 * @file lane.h
 * @brief Includes the lane detection, processing and departure algorithms
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 *---------------------------------------------------------------------------*/

#ifndef LANE_H
#define LANE_H

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include "log.h"

using namespace cv;

/* @brief A lane line detection and processing class
 */
class LaneDetector {

private:

  Mat* raw;     // raw image
  Mat annot;    // annotated image
  Mat gray;     // grayscale image
  Mat roi;      // region of interest 
  Mat roi_mask; // the white mask for the region of interest
  
  // rectangle which defines the roi within the raw frame
  Point roi_pts[4];

  // lane detected points in frame, ready for drawing
  Point left_pt1;
  Point left_pt2; 
  Point right_pt1;
  Point right_pt2;

  // metrics to track
  unsigned int frame_num;
  unsigned int lines_detected;
  double proc_min, proc_max;
  double proc_start, proc_end, proc_elapsed;

  // lane detection
  bool is_left_found, is_right_found;
  unsigned int vcenter;
  unsigned int offset;

  // a friend helper function
  friend bool intersection(Point2f o1, Point2f p1, 
               Point2f o2, Point2f p2, Point2f& r);
  
  // checks to see if a point is within bounds of annot image
  bool is_inside_annot(Point p);

public:
  
  // default constructor
  LaneDetector();

  // methods -- further explanation in lane.cpp 
  void input_image(Mat& img);
  void detect();
  void annotate();
  void show();
  void hough_transform(Vec4i& left, Vec4i& right);

  // getters inline 
  double get_proc_elapsed() { return proc_elapsed; }
  double get_proc_min() { return proc_min; }
  double get_proc_max() { return proc_max; }
  unsigned int get_frame_num() { return frame_num; }
  unsigned int get_lines_detected() { return lines_detected; }
  void get_annot(Mat& annotated_return) { annotated_return = annot.clone(); }

};

// helper function
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f& r);

#endif  // LANE_H

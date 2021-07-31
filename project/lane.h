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
//#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
//#include <opencv2/cudaarithm.hpp>
//#include <opencv2/cudaimgproc.hpp>

#include "log.h"

using namespace cv;

class LaneDetector {

  private:

    Mat* raw;     // raw image
    Mat gray;     // grayscale image
    Mat roi;      // region of interest 
    Mat roi_mask; // the white mask for the region of interest
    
    // rectangle which defines the roi within the raw frame
    Point roi_pts[4];

    // lane detected points in raw frame
    Point left_pt1;
    Point left_pt2; 
    Point right_pt1;
    Point right_pt2;

    // metrics to track
    unsigned int frame_num;
    unsigned int lines_detected;

    // lane detection booleans
    bool is_left_found = false;
    bool is_right_found = false;

    // show pipeline in annotation
    bool show_pipeline;

    // a friend helper function
    friend bool intersection(Point2f o1, Point2f p1, 
                 Point2f o2, Point2f p2, Point2f& r);
    
    // checks to see if a point is within bounds of raw image
    bool is_inside_raw(Point p);

    // filters by angle
    char filter_by_angle(float angle);

  public:
    
    // default constructor
    LaneDetector() = delete;
    LaneDetector(bool show);

    // methods
    void input_image(Mat& img) { raw = &img; }
    void detect();
    void annotate();
    void hough_transform(Vec4i& left, Vec4i& right);
    void hough_transform_P(Vec4i& left, Vec4i& right);


};

// helper function
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f& r);

#endif  // LANE_H

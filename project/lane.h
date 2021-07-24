/* ----------------------------------------------------------------------------
 * @file lane.h
 * @brief
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 * @resources
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include "log.h"

void lane_detect(cv::Mat gray_img, cv::Mat* ret, cv::Rect* roibox);
void crop_ROI(cv::Mat img, cv::Mat* ret, cv::Rect* roibox);


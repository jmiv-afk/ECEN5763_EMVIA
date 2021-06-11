/* ---------------------------------------------------------------------------- 
 * @file my_opencv.c
 * @brief Basic opencv program 
 *
 * Opens an image in a new window and draws a 4 pixel wide border around the 
 * image and a single pixel yellow crosshair down the center of the image.
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 *
 * @resources
 *  - O'Reilly Learning OpenCV 3
 *  - Siewert's simplecv.cpp program
 *  - OpenCV demo: https://docs.opencv.org/3.3.1/d3/d2e/copyMakeBorder_demo_8cpp-example.html#a3
 *---------------------------------------------------------------------------*/

#include <iostream>
#include <cstdint>
#include <string>

// OpenCV includes
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

int main(int argc, char** argv) 
{
  int border = 4;
  Scalar yellow(0, 255, 255);
  std::string window_name = "Border and crosshairs";

  // sanitize inputs (expect path to image):
  if (argc < 2) 
  {
    std::cout << "Usage: ./my_opencv /input_path /output_path_optional\n";
    return -1;
  }
  
  Mat img = imread(argv[1], IMREAD_UNCHANGED);

  if (img.empty())
  {
    std::cout << argv[1] << " not found\n";
    return -1;
  }

  namedWindow(window_name, WINDOW_AUTOSIZE);


  Mat dst(img.rows + border*2, img.cols + border*2, img.type());
  
  copyMakeBorder(img, dst, border, border, border, border, 
                 BORDER_CONSTANT, yellow);

  // vertical line
  line(dst,                        
       Point(dst.cols/2, 0),        
       Point(dst.cols/2, dst.rows),  
       yellow,                      
       1,                 
       LINE_4); 
  
  // horizontal line
  line(dst,                        
       Point(0, dst.rows/2),        
       Point(dst.cols, dst.rows/2),  
       yellow,                      
       1,                 
       LINE_4); 

  // annotate with resolution
  putText(dst,
          std::to_string(dst.cols)+"x"+std::to_string(dst.rows),
          Point(0, dst.rows),
          FONT_HERSHEY_PLAIN,
          1,
          Scalar(0,255,0)
         );

  while(1) 
  {
    imshow(window_name, dst);
    char c = (char)waitKey(500);
    if(c == 27 || c == 'q' || c == 'Q') 
    {
      if (argc == 3)
      {
        imwrite(argv[2], dst);
      }
      break;
    }
  }

  return 0;
}

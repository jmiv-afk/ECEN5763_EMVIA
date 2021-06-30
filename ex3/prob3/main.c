/* ---------------------------------------------------------------------------- 
 * @file main.c
 * @brief RGB frame differencing proof of concept 
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 *---------------------------------------------------------------------------*/

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include"imageio.h"
#include"imageproc.h"

#define INPUT_FOLDER    ("PPM_in")
#define OUTPUT_FOLDER   ("PPM_out")
#define FILENAME_LEN    (30)
#define BG_UPDATE       (5)   // frames to diff before updating bg 
#define STARTFRAME      (100)  // starting frame no
#define NFRAMES         (400)  // total num frames to analyze

// 
// the main application
//
int main(int argc, char** argv)  {

  ppm_img_t *bg = NULL;
  ppm_img_t *fr = NULL;
  ppm_img_t *diff = NULL;

  char filename[FILENAME_LEN];
  int i=STARTFRAME;

  // read first image (set background)
  sprintf(filename, "%s/DRLS%04d.ppm", INPUT_FOLDER, i);
  bg = read_ppm(filename);
  if (!bg) {
    printf("bg ptr is NULL");
    return -1;
  }

  // allocate diff
  diff = new_ppm(bg->hres, bg->vres);

  for (i=STARTFRAME+1; i<=STARTFRAME+NFRAMES; i++) {

    sprintf(filename, "%s/DRLS%04d.ppm", INPUT_FOLDER, i);
    fr = read_ppm(filename);
    if (!fr) {
      printf("bg ptr is NULL");
      return -1;
    }

    if(rgb_diff(fr, bg, diff)) {
      printf("rgb_diff fail %d\n", i);
      return -1;
    }

    sprintf(filename, "%s/DRLS%04d_out.ppm", OUTPUT_FOLDER, i);
    if(write_ppm(filename, diff)) {
      printf("write_ppm fail\n");
      return -1;
    }

    // update background every BG_UPDATE frames
    if ( ((i-1) % BG_UPDATE) == 0) {
      yeet_ppm(&bg);
      bg = fr;
    } else {
      yeet_ppm(&fr);
    }
  }
  
} // end main


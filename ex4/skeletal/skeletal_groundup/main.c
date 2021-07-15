/* ---------------------------------------------------------------------------- 
 * @file main.c
 * @brief Skeletal transformation from ground-up
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
#define OUTPUT_FOLDER   ("PGM_out")
#define FILENAME_LEN    (30)
#define BG_UPDATE       (1000000) // frames to diff before updating bg 
#define STARTFRAME      (60)      // starting frame no
#define LASTFRAME       (965)     // final frame no 
#define THRESH          (10)

// 
// the main application
//
int main(int argc, char** argv)  {

  ppm_img_t *bg = NULL;
  ppm_img_t *fr = NULL;
  pgm_img_t *diff = NULL;
  pgm_img_t *skel = NULL;
  char filename[FILENAME_LEN];
  int i=STARTFRAME;

  // read first image (set background)
  sprintf(filename, "%s/%04d.ppm", INPUT_FOLDER, i);
  bg = read_ppm(filename);
  if (!bg) {
    printf("bg ptr is NULL");
    return -1;
  }

  for (i=STARTFRAME; i<=LASTFRAME; i++) {

    // read input frame
    sprintf(filename, "%s/%04d.ppm", INPUT_FOLDER, i);
    fr = read_ppm(filename);
    if (!fr) {
      printf("bg ptr is NULL");
      return -1;
    }

    // do rgb differencing
    diff = rgb_diff(fr, bg, THRESH);

    skel = skeletal_strip(diff);
    if (!skel) {
      printf("skeletal_strip fail %d\n", i);
      return -1;
    }

    sprintf(filename, "%s/%04d.pgm", OUTPUT_FOLDER, i);
    if (write_pgm(filename, skel)) {
      printf("write_pgm fail\n");
      return -1;
    }
    
    // deallocate unused heap vars
    yeet_pgm(&diff);
    yeet_pgm(&skel);

    // update background every BG_UPDATE frames
    if ( ((i-1) % BG_UPDATE) == 0) {
      yeet_ppm(&bg);
      bg = fr;
    } else {
      yeet_ppm(&fr);
    }

  }
  
} // end main


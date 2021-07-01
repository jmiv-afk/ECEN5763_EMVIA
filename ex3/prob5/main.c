/* ---------------------------------------------------------------------------- 
 * @file main.c
 * @brief converts a series of .ppm frames to .pgm frames based on G channel
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 *---------------------------------------------------------------------------*/

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<omp.h>
#include"imageio.h"
#include"imageproc.h"

#define INPUT_FOLDER    ("PPM_in")
#define OUTPUT_FOLDER   ("PGM_out")
#define FILENAME_LEN    (30)

// 
// the main application
//
int main(int argc, char** argv)  {

  ppm_img_t *img = NULL;
  pgm_img_t *gray = NULL;
  pgm_img_t *grayfilt = NULL;

  char filename[FILENAME_LEN];

  for (int i=1; i<=1260; i++) {

    sprintf(filename, "%s/DRLS%04d.ppm", INPUT_FOLDER, i);
    img = read_ppm(filename);
    if (!img) {
      printf("img ptr is NULL");
      //return -1;
    }

    // convert ppm to pgm using G channel
    gray = ppm_to_pgm(img, 'g');    
    yeet_ppm(&img);
    if(!gray) {
      printf("ppm_to_pgm fail\n");
      //return -1;
    }

    // use median filter
    grayfilt = median_filter(gray);
    yeet_pgm(&gray);
    if (!grayfilt) {
      printf("median_filter fail\n");
      //return -1;
    }

    // annotate COM based on thresh
    if (com_annotate(grayfilt, 235)) {
      printf("com_annotate fail\n");  
      //return -1;
    }

    // write pgm to output file
    sprintf(filename, "%s/DRLS%04d_out.pgm", OUTPUT_FOLDER, i);
    if(write_pgm(filename, grayfilt)) {
      printf("write_pgm fail\n");
      //return -1;
    }

    yeet_pgm(&grayfilt);

  } // end for
  
  return 0;
} // end main


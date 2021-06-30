/* ---------------------------------------------------------------------------- 
 * @file imageproc.c
 * @brief image processing functions for ppm and pgm types 
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 *---------------------------------------------------------------------------*/

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<errno.h>
#include<omp.h>
#include"imageio.h"
#include"imageproc.h"

// see .h for more details
int rgb_diff(ppm_img_t *const frame, ppm_img_t *const bg, ppm_img_t *diff) {
  
  int i;
  int temp;
  if (!frame || !bg || !diff) return -1;

  // use openmp for speedup:
#pragma omp parallel for num_threads(5) \
  default(none) private(i, temp) shared(diff)
  for (i=0; i<bg->hres*bg->vres; i++) {

    // rgb subtraction on ea. channel and keep within 0-255 intensity
    temp = frame->pixel[i].r - bg->pixel[i].r;
    if (temp < 0) temp = 0;
    if (temp > 255) temp = 255;
    diff->pixel[i].r = temp;

    temp = frame->pixel[i].g - bg->pixel[i].g;
    if (temp < 0) temp = 0;
    if (temp > 255) temp = 255;
    diff->pixel[i].g = temp;

    temp = frame->pixel[i].b - bg->pixel[i].b;
    if (temp < 0) temp = 0;
    if (temp > 255) temp = 255;
    diff->pixel[i].b = temp;
    
  }
  return 0;
}

// see .h for more details
int kernel_3x3(int i0, int j0, pgm_img_t *pgm, uint8_t *P) {

  if (i0<1 || j0<1 || !pgm || !P) {
    printf("kernel_3x3 error");
    return -1;
  }
  
  P[0] = pgm->pixel[i0*pgm->hres+j0];
  P[1] = pgm->pixel[i0*pgm->hres+j0+1];
  P[2] = pgm->pixel[(i0-1)*pgm->hres+j0+1];
  P[3] = pgm->pixel[(i0-1)*pgm->hres+j0];
  P[4] = pgm->pixel[(i0-1)*pgm->hres+j0-1];
  P[5] = pgm->pixel[i0*pgm->hres+j0-1];
  P[6] = pgm->pixel[(i0+1)*pgm->hres+j0-1];
  P[7] = pgm->pixel[(i0+1)*pgm->hres+j0];
  P[8] = pgm->pixel[(i0+1)*pgm->hres+j0+1];

  return 0;
}

// see .h for more details
int median_filter(pgm_img_t *pgm) {
  
  static uint8_t hist[256];
  uint8_t kernel[9];
  int i; int sum;

  // allocate space for the new filtered pixels 
  uint8_t* pixel_filt = malloc(sizeof(uint8_t)*pgm->vres*pgm->hres);
  if (!pixel_filt) {
    perror("pixel_filt malloc failed");
    return -1;
  }

  // zero out hist
  memset(&hist, 0, 256);

  for (int r=1; r < pgm->vres-1; r++) {
    for (int c=1; c < pgm->hres-1; c++) {

      kernel_3x3(r, c, pgm, kernel);

      for (int m=0; m<9; m++) hist[kernel[m]]++; 

      // no need to sort, extracts 5th largest neighbor in kernel
      i = 0; 
      sum = 0;

      while(sum < 5) {
        sum = sum + hist[i];
        i += 1;
      }

      pixel_filt[r*pgm->hres+c] = i-1;

      for (int m=0; m<9; m++) hist[kernel[m]] = 0;
    }
  }

  // free old pixel data
  free(pgm->pixel);
  // set new pointer to filtered data
  pgm->pixel = pixel_filt;
  
  return 0;
}


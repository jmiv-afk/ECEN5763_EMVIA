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
int com_annotate(pgm_img_t *pgm, uint8_t thresh) {

  // indices of the maxima and minima
  int xmax=-1, xmin=pgm->hres, ymax=-1, ymin=pgm->vres;
  static int xbar=0, ybar=0;
  int i,j;

  if (!pgm) return -1;

  // scan through all pixels and determine x and y max/min extents
  for (i=0; i<pgm->vres; i++) {
    for (j=0; j<pgm->hres; j++) {

      if (pgm->pixel[i*pgm->hres+j] >= thresh) {
        if (i > ymax) ymax = i;
        if (i < ymin) ymin = i;
        if (j > xmax) xmax = j;
        if (j < xmin) xmin = j;
      }
    }
  }

  // only update xbar and ybar if we have valid xmin and xmax
  if (xmin>-1 && xmax<pgm->hres) {
    xbar = (xmax - xmin)/2 + xmin;
  }
  if (ymin>-1 && ymax<pgm->vres) {
    ybar = (ymax - ymin)/2 + ymin;
  }

  // draw horizontal line at ybar
  for (j=0; j<pgm->hres; j++) {
    //pgm->pixel[(ybar-1)*pgm->hres + j] = 255;
    pgm->pixel[(ybar)*pgm->hres + j] = 255;
    //pgm->pixel[(ybar+1)*pgm->hres + j] = 255;
  }

  // draw vertical line at xbar
  for (i=0; i<pgm->vres; i++) {
    //pgm->pixel[i*pgm->hres + xbar-1] = 255;
    pgm->pixel[i*pgm->hres + xbar] = 255;
    //pgm->pixel[i*pgm->hres + xbar+1] = 255;
  }

  return 0;
}

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
pgm_img_t* median_filter(pgm_img_t *pgm) {
  
  uint8_t hist[256];
  uint8_t kernel[9];
  int i, sum, m, r, c;

  if (!pgm) return NULL;

  // zero out hist
  memset(&hist, 0, 256*sizeof(uint8_t));

  // allocate space for the new filtered pixels 
  pgm_img_t* pgm_filt = new_pgm(pgm->hres, pgm->vres);

  for (r=0; r < pgm->vres; r++) {
    for (c=0; c < pgm->hres; c++) {

      // if they are edge pixels, copy them 
      if (r == 0 || r == pgm->vres-1 || c == 0 || c == pgm->hres) {
        pgm_filt->pixel[r*pgm_filt->hres+c] = pgm->pixel[r*pgm->hres+c];
        continue;
      } 

      kernel_3x3(r, c, pgm, &kernel[0]);

      for (m=0; m<9; m++) hist[kernel[m]]++; 

      // no need to sort, extracts 5th largest neighbor in kernel
      i = 0; sum = 0;

      while(sum < 5) {
        sum += hist[i++];
      }

      pgm_filt->pixel[r*pgm_filt->hres+c] = i-1;

      // zero histogram entries that were used
      for (m=0; m<9; m++) hist[kernel[m]] = 0;
    }
  }
  
  
  return pgm_filt;
}


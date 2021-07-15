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
pgm_img_t* skeletal_strip(pgm_img_t *pgm) {

  uint8_t P[9]; 
  int sigma, chi, i, j;
  int is_modified;
  //int iter = 0;

  if (!pgm) return NULL;

  pgm_img_t *prev = copy_pgm(pgm);
  pgm_img_t *stripped = copy_pgm(pgm);
  pgm_img_t *temp;

  do {

    // The final code uses a cyclical thinning algorithm which strips proper 
    // points from each cardinal kernel direction in subsequent passes.
    // This makes the final skeleton more centered inside the figure. 

    is_modified = 0;

    // strip north points
    for (i=1; i<pgm->vres-1; i++) {
      for (j=1; j<pgm->hres-1; j++) {

        kernel_3x3(i, j, prev, P);
        sigma = P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7]+P[8];   
        chi = (int)(P[1]!=P[3]) + (int)(P[3]!=P[5]) +
              (int)(P[5]!=P[7]) + (int)(P[7]!=P[1]) +
              2*( (int)((P[2]>P[1]) && (P[2]>P[3])) + 
                  (int)((P[4]>P[3]) && (P[4]>P[5])) + 
                  (int)((P[6]>P[5]) && (P[6]>P[7])) +
                  (int)((P[8]>P[7]) && (P[8]>P[1])) );
        if ( (P[3]==0) && (P[0]==255) && (P[7]==255) && (chi==2) && (sigma!=255)) {
          stripped->pixel[i*pgm->hres+j] = 0;
          is_modified |= 1;
        } else {
          stripped->pixel[i*pgm->hres+j] = P[0];
        }
      }
    }

    // ptr swap pattern
    temp = prev;
    prev = stripped;
    stripped = temp;

    // strip east points
    for (i=1; i<pgm->vres-1; i++) {
      for (j=1; j<pgm->hres-1; j++) {

        kernel_3x3(i, j, prev, P);
        sigma = P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7]+P[8];   
        chi = (int)(P[1]!=P[3]) + (int)(P[3]!=P[5]) +
              (int)(P[5]!=P[7]) + (int)(P[7]!=P[1]) +
              2*( (int)((P[2]>P[1]) && (P[2]>P[3])) + 
                  (int)((P[4]>P[3]) && (P[4]>P[5])) + 
                  (int)((P[6]>P[5]) && (P[6]>P[7])) +
                  (int)((P[8]>P[7]) && (P[8]>P[1])) );
        if ( (P[1]==0) && (P[0]==255) && (P[5]==255) && (chi==2) && (sigma!=255)) {
          stripped->pixel[i*pgm->hres+j] = 0;
          is_modified |= 1;
        } else {
          stripped->pixel[i*pgm->hres+j] = P[0];
        }
      }
    }

    // ptr swap pattern
    temp = prev;
    prev = stripped;
    stripped = temp;

    // strip south points
    for (i=1; i<pgm->vres-1; i++) {
      for (j=1; j<pgm->hres-1; j++) {

        kernel_3x3(i, j, prev, P);
        sigma = P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7]+P[8];   
        chi = (int)(P[1]!=P[3]) + (int)(P[3]!=P[5]) +
              (int)(P[5]!=P[7]) + (int)(P[7]!=P[1]) +
              2*( (int)((P[2]>P[1]) && (P[2]>P[3])) + 
                  (int)((P[4]>P[3]) && (P[4]>P[5])) + 
                  (int)((P[6]>P[5]) && (P[6]>P[7])) +
                  (int)((P[8]>P[7]) && (P[8]>P[1])) );
        if ( (P[7]==0) && (P[0]==255) && (P[3]==255) && (chi==2) && (sigma!=255)) {
          stripped->pixel[i*pgm->hres+j] = 0;
          is_modified |= 1;
        } else {
          stripped->pixel[i*pgm->hres+j] = P[0];
        }
      }
    }

    // ptr swap pattern
    temp = prev;
    prev = stripped;
    stripped = temp;

    // strip west points
    for (i=1; i<pgm->vres-1; i++) {
      for (j=1; j<pgm->hres-1; j++) {

        kernel_3x3(i, j, prev, P);
        sigma = P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7]+P[8];   
        chi = (int)(P[1]!=P[3]) + (int)(P[3]!=P[5]) +
              (int)(P[5]!=P[7]) + (int)(P[7]!=P[1]) +
              2*( (int)((P[2]>P[1]) && (P[2]>P[3])) + 
                  (int)((P[4]>P[3]) && (P[4]>P[5])) + 
                  (int)((P[6]>P[5]) && (P[6]>P[7])) +
                  (int)((P[8]>P[7]) && (P[8]>P[1])) );
        if ( (P[5]==0) && (P[0]==255) && (P[1]==255) && (chi==2) && (sigma!=255)) {
          stripped->pixel[i*pgm->hres+j] = 0;
          is_modified |= 1;
        } else {
          stripped->pixel[i*pgm->hres+j] = P[0];
        } 
      }
    }

    // ptr swap pattern
    temp = prev;
    prev = stripped;
    stripped = temp;

    //printf("iter %d, is_modified %d\n", iter++, is_modified);

    /* 
    // The following (commented out) is the "obvious algorithm" from 
    // Davies Ch. 9, 4th ed, Listing 9.11 
    // This is the code that I started with:
    for (i=1; i<pgm->vres-1; i++) {
      for (j=1; j<pgm->hres-1; j++) {

        kernel_3x3(i, j, stripped, P);
        sigma = P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7]+P[8];   
        chi = (int)(P[1]!=P[3]) + (int)(P[3]!=P[5]) +
              (int)(P[5]!=P[7]) + (int)(P[7]!=P[1]) +
              2*( (int)((P[2]>P[1]) && (P[2]>P[3])) + 
                  (int)((P[4]>P[3]) && (P[4]>P[5])) + 
                  (int)((P[6]>P[5]) && (P[6]>P[7])) +
                  (int)((P[8]>P[7]) && (P[8]>P[1])) );
        if ((P[0]==255) && (chi==2) && (sigma!=255)) {
          stripped->pixel[i*pgm->hres+j] = 0;
          is_modified = 1;
        } 
      }
    }
    */

  } while(is_modified);

  yeet_pgm(&stripped);

  return prev;
}

// see .h for more details
pgm_img_t* rgb_diff(ppm_img_t *const frame, ppm_img_t *const bg, int thresh) {
  
  int i;
  double temp;
  if (!frame || !bg) return NULL;

  pgm_img_t *diff = new_pgm(frame->hres, frame->vres);

  for (i=0; i<bg->hres*bg->vres; i++) {

    temp = 0.299*abs(frame->pixel[i].r-bg->pixel[i].r) + 
           0.587*abs(frame->pixel[i].g-bg->pixel[i].g) + 
           0.114*abs(frame->pixel[i].b-bg->pixel[i].b) ; 
    if (temp >= thresh) {
      diff->pixel[i] = 255;
    } else { 
      diff->pixel[i] = 0;
    }

  }

  return diff;
}

// see .h for more details
int kernel_3x3_rgb(int i0, int j0, ppm_img_t *ppm, pixl_t *P) {

  if (i0<1 || j0<1 || !ppm || !P) {
    printf("kernel_3x3 error");
    return -1;
  }
  
  P[0] = ppm->pixel[i0*ppm->hres+j0];
  P[1] = ppm->pixel[i0*ppm->hres+j0+1];
  P[2] = ppm->pixel[(i0-1)*ppm->hres+j0+1];
  P[3] = ppm->pixel[(i0-1)*ppm->hres+j0];
  P[4] = ppm->pixel[(i0-1)*ppm->hres+j0-1];
  P[5] = ppm->pixel[i0*ppm->hres+j0-1];
  P[6] = ppm->pixel[(i0+1)*ppm->hres+j0-1];
  P[7] = ppm->pixel[(i0+1)*ppm->hres+j0];
  P[8] = ppm->pixel[(i0+1)*ppm->hres+j0+1];

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
ppm_img_t* median_filter_rgb(ppm_img_t *ppm) {
  
  pixl_t hist[256];
  pixl_t kernel[9];
  int i, sum, m, r, c;

  if (!ppm) return NULL;

  // zero out hist
  memset(&hist, 0, 256*sizeof(pixl_t));

  // allocate space for the new filtered pixels 
  ppm_img_t* ppm_filt = new_ppm(ppm->hres, ppm->vres);

  for (r=0; r < ppm->vres; r++) {
    for (c=0; c < ppm->hres; c++) {

      // if they are edge pixels, copy them 
      if (r == 0 || r == ppm->vres-1 || c == 0 || c == ppm->hres) {
        ppm_filt->pixel[r*ppm_filt->hres+c].r = ppm->pixel[r*ppm->hres+c].r;
        ppm_filt->pixel[r*ppm_filt->hres+c].g = ppm->pixel[r*ppm->hres+c].g;
        ppm_filt->pixel[r*ppm_filt->hres+c].b = ppm->pixel[r*ppm->hres+c].b;
        continue;
      } 

      kernel_3x3_rgb(r, c, ppm, &kernel[0]);

      for (m=0; m<9; m++) {
        hist[kernel[m].r].r++;
        hist[kernel[m].g].g++;
        hist[kernel[m].b].b++;
      }

      // no need to sort, extracts 5th largest neighbor in kernel
      i = 0; sum = 0;
      while(sum < 5) {
        sum += hist[i++].r;
      }
      ppm_filt->pixel[r*ppm_filt->hres+c].r = i-1;

      i = 0; sum = 0;
      while(sum < 5) {
        sum += hist[i++].g;
      }
      ppm_filt->pixel[r*ppm_filt->hres+c].g = i-1;

      i = 0; sum = 0;
      while(sum < 5) {
        sum += hist[i++].b;
      }
      ppm_filt->pixel[r*ppm_filt->hres+c].b = i-1;

      // zero histogram entries that were used
      for (m=0; m<9; m++) {
        hist[kernel[m].r].r = 0;
        hist[kernel[m].g].g = 0;
        hist[kernel[m].b].b = 0;
      }
    }
  }
  
  
  return ppm_filt;
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


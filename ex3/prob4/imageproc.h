/* ---------------------------------------------------------------------------- 
 * @file imageproc.h
 * @brief image processing functions for ppm and pgm types 
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 *---------------------------------------------------------------------------*/

#ifndef IMAGEPROC_H_
#define IMAGEPROC_H_

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<errno.h>
#include"imageio.h"


/* @brief Performs frame difference for R,G,B channels
 *
 * @param *frame, the frame to analyze
 * @param *bg, the frame to use as background
 * @param *diff, where to store resulting difference frame
 * @return 0 on success, -1 on error
 */
int rgb_diff(ppm_img_t *const frame, ppm_img_t *const bg, ppm_img_t *diff);

/* @brief Retrieves the 3x3 kernel P[0...8] from PGM
 *
 * The 3x3 kernel notation for P0 as pixel of interest: 
 *      P4 | P3 | P2
 *      P5 | P0 | P1
 *      P6 | P7 | P8
 *
 * @param i0, the row of P0 (pixel of interest)
 * @param j0, the column of P0 (pixel of interest)
 * @param *pgm, a pointer to a pgm_img_t
 * @param *P, a pointer to a uint8_t array with minimum length of 9.
 *            Holds the return kernel pixel values.
 * @return 0 on success, -1 on error
 */
int kernel_3x3(int i0, int j0, pgm_img_t *pgm, uint8_t *P);


/* @brief Applies a median filter to pgm
 *
 * @param *pgm, a pointer to a pgm_img_t
 * @return 0 on success, -1 on error
 */
int median_filter(pgm_img_t *pgm); 

#endif // IMAGEPROC_H_

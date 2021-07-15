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

/* @brief skeletal stripping using algorithm defined in 
 *        E.R. Davies, 4th ed. (listings 9.13/9.14)
 *
 * @param *pgm, a pointer to the pgm_img_t type to skeletal strip
 *              Note that the pgm input shall be a binary pgm with values of 
 *              only 0 and 255
 * @return pgm_img_t*, pointer to the stripped image
 *         NULL upon error
 */
pgm_img_t* skeletal_strip(pgm_img_t *pgm);

/* @brief Performs frame difference for R,G,B channels
 *
 * @param *frame, the frame to analyze
 * @param *bg, the frame to use as background
 * @param thresh, the threshold for extracting foreground
 * @return *pgm_img_t, the resulting difference frame (binary-bitmap pgm)
 *          NULL on error
 */
pgm_img_t* rgb_diff(ppm_img_t *const frame, ppm_img_t *const bg, int thresh);

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
int kernel_3x3_rgb(int i0, int j0, ppm_img_t *ppm, pixl_t *P); 


/* @brief Applies a median filter to pgm/ppm
 *
 * @param *pgm, a pointer to a pgm_img_t
 * @param *ppm, a pointer to a ppm_img_t
 * @return *pgm_img_t, a pointer to a filtered pgm_img_t
 *          NULL on error
 */
pgm_img_t *median_filter(pgm_img_t *pgm); 
ppm_img_t *median_filter_rgb(ppm_img_t *ppm); 

#endif // IMAGEPROC_H_

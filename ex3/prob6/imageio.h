/* ---------------------------------------------------------------------------- 
 * @file imageio.h
 * @brief PPM/PGM image input/output functions
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 *---------------------------------------------------------------------------*/

#ifndef IMAGEIO_H_
#define IMAGEIO_H_

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

// the color pixel structure with r,g,b components
typedef struct {
  uint8_t r, g, b;
} pixl_t;

// the ppm image format (rgb pixel intensity)
typedef struct {
  unsigned int hres, vres;
  pixl_t* pixel;
} ppm_img_t;

// the pgm image format (single channel pixel intensity)
typedef struct {
  unsigned int hres, vres;
  uint8_t* pixel;
} pgm_img_t;

/* @brief Creates (allocates heap) for a new ppm/pgm
 *
 * @param hres, horizontal resolution of the pixmap
 * @param vres, vertical resolution of the pixmap
 * @return pointer to the ppm/pgm (NULL on error)
 */
ppm_img_t* new_ppm(int hres, int vres);
pgm_img_t* new_pgm(int hres, int vres);

/* @brief Destroyes (yeets) a previously allocated ppm/pgm
 *
 * @param **ppm, pointer to the ppm/pgm to be deleted
 * @return None 
 */
void yeet_ppm(ppm_img_t** ppm);
void yeet_pgm(pgm_img_t** pgm);

/* @brief creates a deep copy of a ppm
 * @param the ppm to copy
 * @return the copy
 *          NULL on error
 */
ppm_img_t* copy_ppm(ppm_img_t* old);

/* @brief Reads a .ppm image into a ppm_img_t type 
 *
 * @param *file, name of the file
 * @return *ppm_img_t, a pointer to the ppm pixmap 
 *          (NULL on error)
 */
ppm_img_t* read_ppm(char* file);

/* @brief Converts a ppm structure to a pgm structure
 * 
 * @param *ppm, a pointer to the ppm to convert
 * @param mapping, a character 'r', 'g', or 'b' to use to carry out
 *                 the mapping
 * @return *pgm_img_t, a pointer to the converted pgm pixmap
 */
pgm_img_t* ppm_to_pgm(ppm_img_t *ppm, char mapping);

/* @brief Writes .PGM data to the file 
 *
 * @param *file, name of the file
 * @param *pgm, a pointer to the pgm to be written
 * @return 0 on success, -1 on error
 */
int write_pgm(char *file, pgm_img_t *pgm); 

/* @brief Writes .PPM data to the file 
 *
 * @param *file, name of the file
 * @param *ppm, a pointer to the ppm to be written
 * @return 0 on success, -1 on error
 */
int write_ppm(char *file, ppm_img_t *ppm);

#endif // IMAGEIO_H_

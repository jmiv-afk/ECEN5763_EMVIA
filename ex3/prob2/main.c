/* ---------------------------------------------------------------------------- 
 * @file median_filter.c
 * @brief Application of the median filter to G-band greymap 
 *
 * Usage: ./median_filter input_file.ppm output_file.ppm <y/n>
 *        $ make run
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

// the main application
int main(int argc, char** argv)  {

  ppm_img_t* ppm = NULL;
  pgm_img_t* pgm = NULL;

  if (argc < 4) {
    printf("Usage: ./median_filter input_file.ppm output_file.pgm y\n");
    return -1;
  }

  // read the .ppm file to ppm_img_t struct
  ppm = read_ppm(argv[1]);
  if (!ppm) {
    perror("read_ppm fail");
    return -1;
  }

  // convert ppm_img_t to pgm_img_t 
  pgm = ppm_to_pgm(ppm, 'g');
  if (!pgm) {
    printf("error: ppm_to_pgm fail\n");
    return -1;
  }

  // do median filtering on g-band (conditionally)
  if (argv[3][0] == 'y') {
    if(median_filter(pgm)) {
      printf("median_filter error\n");
      return -1;
    }
  } 

  // write to output pgm file
  if (write_pgm(argv[2], pgm)) {
    printf("error: write_pgm fail\n");
    return -1;
  }

  // free memory allocated with malloc()
  yeet_ppm(&ppm);
  yeet_pgm(&pgm);

  return 0;
} // end main



/* ---------------------------------------------------------------------------- 
 * @file median_filter.c
 * @brief Application of the median filter to G-band greymap 
 *
 * Usage: ./median_filter input_file.ppm output_file.ppm
 *
 * @author Jake Michael, jami1063@colorado.edu
 * @course ECEN 5763: EMVIA, Summer 2021
 *
 * @resources
 *---------------------------------------------------------------------------*/

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<errno.h>

#define BUFSIZE (16)

// the pixel structure with r,g,b components
typedef struct {
  int r, g, b;
} pixl;

// the ppm image format
typedef struct {
  int hres, vres;
  pixl* dat;
} ppm_img;

int read_ppm(FILE *fp, ppm_img* ppm) {

  char buf[BUFSIZE];
  int hres, vres, maxval;
  
  if (!fp) return -1;
  
  // read "P6" magic numbers
  if (1 != fscanf(fp, "%s", buf)) {
    perror("fscanf");
    return -1;
  }

  if (buf[0] != 'P' || buf[1] != '6') {
    perror("magic numbers not found");
    return -1;
  }

  if (2 != fscanf(fp, "%u %u", &hres, &vres)) {
    perror("hres and vres not found");
    return -1;
  }

  if (1 != fscanf(fp, "%u", &maxval)) {
    perror("maxval not found");
    return -1;
  }
  
  // malloc the space for the ppm image data
  ppm = (ppm_img*) malloc(hres*vres*sizeof(pixl));

  // read in the ppm image data
  if (maxval == 255) {
    for (int i = 0; i < vres*hres; i++) {
      fread(fp, 1, ppm_img);
    }
  } else if (maxval == 65535) {
    for (int i = 0; i < vres*hres; i++) {
      fread(fp, 2, ppm_img);
    }
  }


  //printf("%s \n", buf);
  //printf("hres: %u | vres: %u \n", hres, vres);
  //printf("maxval: %u \n", maxval);

  return 0;
}

int main(int argc, char** argv)  {

  FILE *fp_in, *fp_out; 
  ppm_img *ppm;

  //
  //if (argc < 3) {
  //  printf("Usage: ./median_filter input_file.ppm output_file.ppm\n");
  //  return -1;
  //}
  //

  fp_in = fopen("DRLS001.ppm", "r");
  if (fp_in == NULL) {
    perror("fp_in");
    return -1;
  }

  int ret = read_ppm(fp_in, ppm);

} // end main



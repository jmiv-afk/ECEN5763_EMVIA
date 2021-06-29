/* ---------------------------------------------------------------------------- 
 * @file imageio.c
 * @brief PPM/PGM image input/output functions
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

// see .h for more details
ppm_img_t* new_ppm(int hres, int vres) {
  
  if (hres<1 || vres<1) return NULL;

  ppm_img_t* new_ppm = (ppm_img_t*) malloc(sizeof(ppm_img_t));
  if (!new_ppm) {
    perror("cannot malloc new_ppm");
    return NULL;
  }
  new_ppm->hres = hres; new_ppm->vres = vres;
  new_ppm->pixel = (pixl_t*) malloc(sizeof(pixl_t)*vres*hres);
  if (!new_ppm->pixel) {
    perror("cannot malloc new_ppm->pixel");
    return NULL;
  }
  return new_ppm; 
}

// see .h for more details
pgm_img_t* new_pgm(int hres, int vres) {
  
  if (hres<1 || vres<1) return NULL;

  pgm_img_t* new_pgm = (pgm_img_t*) malloc(sizeof(pgm_img_t));
  if (!new_pgm) {
    perror("cannot malloc new_pgm");
    return NULL;
  }
  new_pgm->hres = hres; new_pgm->vres = vres;
  new_pgm->pixel = (uint8_t*) malloc(sizeof(uint8_t)*vres*hres);
  if (!new_pgm->pixel) {
    perror("cannot malloc new_pgm->pixel");
    return NULL;
  }
  return new_pgm;
}

// see .h for more details
void yeet_ppm(ppm_img_t* ppm) {

  free(ppm->pixel);
  free(ppm);
}

// see .h for more details
void yeet_pgm(pgm_img_t* pgm) {

  free(pgm->pixel);
  free(pgm);
}

// see .h for more details
ppm_img_t* read_ppm(char* file) {

  char buf[2];
  int hres, vres, maxval;
  ppm_img_t* ppm = NULL;

  FILE *fp = fopen(file, "r");
  if (!fp) {
    perror("fopen fail"); return NULL;
  }
  
  // read "P6" magic numbers
  if (1 != fscanf(fp, "%s", buf)) {
    perror("fscanf"); return NULL;
  }
  if (buf[0] != 'P' || buf[1] != '6') {
    perror("magic numbers not found"); return NULL;
  }

  // read resolution
  if (2 != fscanf(fp, "%u %u", &hres, &vres)) {
    perror("hres and vres not found"); return NULL;
  }

  // read maxval
  if (1 != fscanf(fp, "%u", &maxval)) {
    perror("maxval not found"); return NULL;
  }
  
  // allocate the ppm 
  ppm = new_ppm(hres, vres);
  if (!ppm) {
    printf("error: cannot get new ppm\n"); return NULL;
  }

  // read in the ppm image data
  if (maxval == 255) {
    for (int i = 0; i < vres*hres; i++) {
      // binary fread 3 bytes and place into pixel array:
      int ret = fread(&(ppm->pixel[i]), 1, 3, fp);
      if (ret != 3) {
        perror("fread error"); return NULL;
      }
    }

  } else {
    printf("cannot read file format other than r,g,b 0-255\n");
    return NULL;
  }

  fclose(fp);
  return ppm;
}

// see .h for more details
pgm_img_t* ppm_to_pgm(ppm_img_t *ppm, char mapping) {

  pgm_img_t* pgm = new_pgm(ppm->hres, ppm->vres);

  // handle error case, unknown mapping
  if (mapping != 'r' && mapping != 'g' && mapping != 'b') {
    printf("ppm_to_pgm mapping unknown\n"); 
    return NULL;
  }
  
  // map to pgm format based on the various channels r,g,b
  if (mapping == 'r') {
    for (int i=0; i<ppm->hres*ppm->vres; i++) {
      pgm->pixel[i] = ppm->pixel[i].r;
    }
  } else if (mapping == 'g') {
    for (int i=0; i<ppm->hres*ppm->vres; i++) {
      pgm->pixel[i] = ppm->pixel[i].g;
    }
  } else { // assume mapping=='b'
    for (int i=0; i<ppm->hres*ppm->vres; i++) {
      pgm->pixel[i] = ppm->pixel[i].b;
    }
  }

  return pgm;
}

// see .h for more details
int write_pgm(char *file, pgm_img_t *pgm) {
  
  int ret;

  FILE *fp = fopen(file, "w");
  if (!fp) {
    perror("fopen fail");
    return -1;
  }

  fprintf(fp, "P5 \r\n");
  fprintf(fp, "%u %u \r\n", pgm->hres, pgm->vres);
  fprintf(fp, "255\n");
  
  // only write out g-band information since we are writing to a .pgm
  for (int i=0; i<pgm->hres*pgm->vres; i++) {
    ret = fwrite(&(pgm->pixel[i]), sizeof(uint8_t), 1, fp);
    if (ret != 1) {
      perror("fwrite error");
      return -1;
    }
  }

  fclose(fp);
  return 0;
}

// see .h for more details
int write_ppm(char *file, ppm_img_t *ppm) {
  
  int ret;

  FILE *fp = fopen(file, "w");
  if (!fp) {
    perror("fopen fail");
    return -1;
  }

  fprintf(fp, "P6 \r\n");
  fprintf(fp, "%u %u \r\n", ppm->hres, ppm->vres);
  fprintf(fp, "255\n");
  
  // only write out g-band information since we are writing to a .pgm
  for (int i=0; i<ppm->hres*ppm->vres; i++) {
    ret = fwrite(&(ppm->pixel[i]), sizeof(pixl_t), 1, fp);
    if (ret != 1) {
      perror("fwrite error");
      return -1;
    }
  }

  fclose(fp);
  return 0;
}


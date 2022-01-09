#ifndef COLORSPACE_H
#define COLORSPACE_H

#include "pngio.h"

void rgb_to_lab ( unsigned char color[3] ) ;
void image_rgb_to_lab ( struct image *img ) ;
void rgb_to_luv (  unsigned char color[3] ) ;
void image_rgb_to_luv ( struct image *img ) ;
void lab_to_rgb ( unsigned char color[3] ) ;
void pal_image_lab_to_rgb ( struct pal_image *pali ) ;
void pal_image_rgb_to_lab ( struct pal_image *pali ) ;
double binouse_rgb_to_lab (unsigned char color[3]) ;

#endif

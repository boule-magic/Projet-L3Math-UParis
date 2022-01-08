#ifndef COLORSPACE_H
#define COLORSPACE_H

#include "pngio.h"

void rgb_to_lab ( unsigned char color[3] ) ;
void image_rgb_to_lab ( struct image *img ) ;
void rgb_to_luv (  unsigned char color[3] ) ;
void image_rgb_to_luv ( struct image *img ) ;

#endif

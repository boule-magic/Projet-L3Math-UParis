#ifndef DITHERING_H
#define DITHERING_H

#include "pngio.h"

int naive_pal_image(struct pal_image* pali, const struct image* img);
int floydSteinberg_pal_image(struct pal_image* pali, struct image* img );
int atkinson_pal_image(struct pal_image* pali, struct image* img);
int ordered_pal_image_nb_rgb(struct pal_image* pali, const struct image* img, int nb_red, int nb_green, int nb_blue);
int ordered_pal_image_static(struct pal_image* pali, const struct image* img);
//void ordered_pal_image_216(struct pal_image* pali, const struct image* img);


#endif

#ifndef CONVERSION_H
#define CONVERSION_H

#include "pngio.h"

int gen_pal_image(struct pal_image* pali, const struct image* img , int num_norme );
int floydSteinberg(struct pal_image* pali, struct image* img , int num_norme );
struct pal_image* new_pal_image(const struct image* img);
struct image* image_scaling(double factor, const struct image* img);

#endif

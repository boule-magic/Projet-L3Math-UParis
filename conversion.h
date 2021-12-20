#ifndef CONVERSION_H
#define CONVERSION_H

#include "pngio.h"

int naive_pal_image(struct pal_image* pali, const struct image* img);
int floydSteinberg_pal_image(struct pal_image* pali, struct image* img );
int atkinson_pal_image(struct pal_image* pali, struct image* img);
int ordered_pal_image(struct pal_image* pali, const struct image* img);
struct pal_image* new_pal_image(const struct image* img);
struct image* image_scaling(double factor, const struct image* img);

#endif

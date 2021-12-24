#ifndef SCALING_H
#define SCALING_H

#include "pngio.h"

struct image* new_scaled_image(double factor, const struct image* img);
struct image* image_scaling(int height, int width, const struct image* img);

#endif

#ifndef PALETTE_H
#define PALETTE_H

#include "pngio.h"

//int indexingImageWithLessThan256Colors (struct pal_image* pali, const struct image* img);

void pal_8 (struct pal_image* pali); //definition d'une palette de 8 couleurs (saturation)
void pal_16 (struct pal_image* pali);
void pal_64 (struct pal_image* pali);
void pal_216 (struct pal_image* pali);
void pal_252 (struct pal_image* pali);
void pal_2 (struct pal_image* pali);
void pal_256 (struct pal_image* pali);
void palette_dynamique (struct pal_image *final, const struct image *initial, int n ) ;
int palette_dynamique_median_cut (struct pal_image* final, const struct image* initial, int palette_len);

int findColorFromPalette(unsigned char color[3], struct pal_image* pali);
int findClosestColorFromPalette(const unsigned char color[3], const struct pal_image* pali);

#endif

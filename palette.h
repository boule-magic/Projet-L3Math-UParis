#ifndef PALETTE_H
#define PALETTE_H

#include "pngio.h"

void pal_8(struct pal_image* pali); //definition d'une palette de 8 couleurs (saturation)
void pal_16(struct pal_image* pali);
void pal_64(struct pal_image* pali);
void pal_216(struct pal_image* pali);
void pal_252(struct pal_image* pali);
void pal_2(struct pal_image* pali);
void pal_256(struct pal_image* pali);

unsigned char findClosestColorFromPalette(const unsigned char* originalPixel, const unsigned char* palette, const int pal_len);
int normeEuclidienne(const unsigned char* C1, const unsigned char* C2);

#endif

#include "pngio.h"

void pal_8(pal_image* pali); //definition d'une palette de 8 couleurs (saturation)
void pal_16(pal_image* pali);
void pal_64(pal_image* pali);
void pal_216(pal_image* pali);
void pal_252(pal_image* pali);
void pal_2(pal_image* pali);
void pal_256(pal_image* pali);
void gen_pal_image(pal_image* pali, const image* img);
pal_image* generation1(const image* img); //bricole à supprimer

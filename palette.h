#include "pngio.h"

void pal_8(struct pal_image* pali); //definition d'une palette de 8 couleurs (saturation)
void pal_16(struct pal_image* pali);
void pal_64(struct pal_image* pali);
void pal_216(struct pal_image* pali);
void pal_252(struct pal_image* pali);
void pal_2(struct pal_image* pali);
void pal_256(struct pal_image* pali);
void gen_pal_image(struct pal_image* pali, const struct image* img);
struct pal_image* generation1(const struct image* img); //bricole à supprimer

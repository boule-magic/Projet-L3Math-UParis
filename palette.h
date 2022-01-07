#ifndef PALETTE_H
#define PALETTE_H

#include "pngio.h"

struct nb_couleur {
    int nb; // pondération (nombre de pixels de cette couleur présent dans l'image)
    unsigned char ucr, ucv, ucb; // (r,g,b)
} ;

struct node {
    struct nb_couleur nc ;
    struct node *left ;
    struct node *right ;
} ;

void palette_8 (struct pal_image* pali); //definition d'une palette de 8 couleurs (saturation)
void palette_16 (struct pal_image* pali);
void palette_64 (struct pal_image* pali);
void palette_216 (struct pal_image* pali);
void palette_252 (struct pal_image* pali);
void palette_2 (struct pal_image* pali);
void palette_256 (struct pal_image* pali);
void palette_dynamique (struct pal_image *final, const struct image *initial, int n ) ;
int palette_dynamique_median_cut (struct pal_image* final, const struct image* initial, int palette_len);

int findColorFromPalette(unsigned char color[3], struct pal_image* pali);
int findClosestColorFromPalette(const unsigned char color[3], const struct pal_image* pali);

#endif

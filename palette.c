#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "palette.h"

struct nb_couleur {
    int nb; // pondération (nombre de pixels de cette couleur présent dans l'image)
    unsigned char ucr, ucv, ucb; // (r,g,b)
} ;

struct node {
    struct nb_couleur nc ;
    struct node *left ;
    struct node *right ;
} ;

//// fonctions utilisées pour la palette dynamique des couleurs les plus présentes
void free_tree ( struct node *n );
struct node * mknode ( struct nb_couleur nc , struct node *left , struct node *right );
struct node * insert(struct nb_couleur newnc, struct node *abr);
void addTheMostCommonColors(struct node *tree, struct nb_couleur* nb_palette, int nb_palette_len_max, int* nb_palette_len); // ajoute les 256 couleurs de l'abr ayant les plus grandes pondérations (les plus présentes dans l'image)


//// fonctions utilisées pour la palette dynamique à "median cut"
void bucket(int nb_bucket, struct pal_image* pali, unsigned char* colors, long int premier, long int dernier); // fonction récursive applicant le "median cut"
int theOneWithTheMostAmplitude(unsigned char* colors, long int premier, long int dernier); // renvoie 0,1 ou 2 respectivement si r, g ou b a la plus grande amplitude
void average_color_calculator(unsigned char* colors, long int premier, long int dernier, unsigned char* average_color); // calcule la couleur moyenne d'un tableau de couleurs (résultat stocké dans average_color)
// fonctions du trie rapide ou trie pivot (C.A.R. Hoare, 1960/1961)
// trie les couleurs d'un tableau entre l'indice "premier" et l'indice "dernier" (inclus) par rapport au r, g ou b en fonction de la valeur de color_reference (0, 1, 2)
void quicksort(unsigned char* colors, long int premier, long int dernier, int color_reference);
void color_swapping(unsigned char* c1, unsigned char* c2);
long int choix_pivot(unsigned char* colors, long int premier, long int dernier);
long int partitionner(unsigned char* colors, long int premier, long int dernier, long int pivot, int color_reference); // comparer les couleurs et les échanges si nécessaire, renvoie l'indice "pivot" : toutes les couleurs "à sa gauche" lui sont inférieures et toutes celles "à sa droite" lui sont supérieures 

// Fonctions utiles
int normeEuclidienne(const unsigned char* C1, const unsigned char* C2); // renvoie la norme euclidienne au carrée (donc pas une norme en réalité)
unsigned char* colors_tab_from_image(const struct image* img);
unsigned char max(unsigned char c1, unsigned char c2);

/* int */
/* indexingImageWithLessThan256Colors(struct pal_image* pali, const struct image* img) { */
/*     if(pali->pal != NULL) { */
/*         free(pali->pal); */
/*     } */
/*     pali->pal = malloc(3*256*sizeof(unsigned char)); */
/*     pali->pal_len = 0; */
/*     for(int i = 0 ; i < img->height ; i++) { */
/* 	for(int j = 0 ; j < img->width ; j++) { */
/* 	    int k = 0; */
/* 	    for(k = 0 ; k < pali->pal_len ; k++) { */
/* 		if(img->data[i][j*4+0] == pali->pal[k*3+0] && */
/* 		   img->data[i][j*4+1] == pali->pal[k*3+1] && */
/* 		   img->data[i][j*4+2] == pali->pal[k*3+2] */
/* 		   ) { */
/* 		    pali->data[i][j] = k; */
/* 		    break; */
/* 		} */
/* 	    } */
/* 	    if(k > 255) { */
/* 		pali->pal_len = -1; */
/* 		return -1; */
/* 	    } else if(k == pali->pal_len) { */
/* 		pali->pal_len++; */
/* 		pali->pal[k*3+0] = img->data[i][j*4+0]; */
/* 		pali->pal[k*3+1] = img->data[i][j*4+1]; */
/* 		pali->pal[k*3+2] = img->data[i][j*4+2]; */
/* 		pali->data[i][j] = k; */
/* 	    } */
/* 	} */
/*     } */
/*     return 1; */
/* } */

void
pal_8(struct pal_image* pali) {
    if(pali->pal != NULL) free(pali->pal);
    pali->pal = malloc(8*3*sizeof(unsigned char));
    pali->pal_len = 8;
    for(int i = 0 ; i <= 1 ; i++) {
        for(int j = 0 ; j <= 1 ; j++) {
	    for(int k = 0 ; k <= 1 ; k++) {
		pali->pal[(i*4+j*2+k)*3] = 255*i;
		pali->pal[(i*4+j*2+k)*3+1] = 255*j;
		pali->pal[(i*4+j*2+k)*3+2] = 255*k;
	    }
	}
    } 
}

void
pal_16(struct pal_image* pali) {
    if(pali->pal != NULL) free(pali->pal);
    pali->pal = malloc(16*3*sizeof(unsigned char));
    pali->pal_len = 16;
    for(int i = 0 ; i <= 1 ; i++) {
        for(int j = 0 ; j <= 1 ; j++) {
	    for(int k = 0 ; k <= 1 ; k++) {
	        for(int l = 0 ; l <= 1 ; l++) {
		    if(i==0 && j==0 && k==0 && l==1) {
			pali->pal[(i*8+j*4+k*2+l)*3] = 192;
			pali->pal[(i*8+j*4+k*2+l)*3+1] = 192;
			pali->pal[(i*8+j*4+k*2+l)*3+2] = 192;
		    }
		    else {
			pali->pal[(i*8+j*4+k*2+l)*3] = 127.5*i + 127.5*l;
			pali->pal[(i*8+j*4+k*2+l)*3+1] = 127.5*j + 127.5*l;
			pali->pal[(i*8+j*4+k*2+l)*3+2] = 127.5*k + 127.5*l;
		    }
		}
	    }
	}
    }
}

void
pal_64(struct pal_image* pali) {
    if(pali->pal != NULL) free(pali->pal);
    pali->pal = malloc(64*3*sizeof(unsigned char));
    pali->pal_len = 64;
    for(int i = 0 ; i <= 3 ; i++) {
        for(int j = 0 ; j <= 3 ; j++) {
	    for(int k = 0 ; k <= 3 ; k++) {
		pali->pal[(i*4*4+j*4+k)*3] = 85*i;
		pali->pal[(i*4*4+j*4+k)*3+1] = 85*j;
		pali->pal[(i*4*4+j*4+k)*3+2] = 85*k;
	    }
	}
    } 
}

void
pal_216(struct pal_image* pali) {
    if(pali->pal != NULL) free(pali->pal);
    pali->pal = malloc(216*3*sizeof(unsigned char));
    pali->pal_len = 216;
    for(int i = 0 ; i <= 5 ; i++) {
        for(int j = 0 ; j <= 5 ; j++) {
	    for(int k = 0 ; k <= 5 ; k++) {
		pali->pal[(i*6*6+j*6+k)*3] = 51*i;
		pali->pal[(i*6*6+j*6+k)*3+1] = 51*j;
		pali->pal[(i*6*6+j*6+k)*3+2] = 51*k;
	    }
	}
    } 
}

void
pal_252(struct pal_image* pali) {
    if(pali->pal != NULL) free(pali->pal);
    pali->pal = malloc(252*3*sizeof(unsigned char));
    pali->pal_len = 252;
    for(int i = 0 ; i <= 5 ; i++) {
        for(int j = 0 ; j <= 5 ; j++) {
	    for(int k = 0 ; k <= 5 ; k++) {
		pali->pal[(i*6*6+j*6+k)*3] = 51*i;
		pali->pal[(i*6*6+j*6+k)*3+1] = 51*k;
		pali->pal[(i*6*6+j*6+k)*3+2] = 51*j;
	    }
	}
    }
    for(int i = 0 ; i <= 5 ; i++) {
        for(int j = 0 ; j <= 5 ; j++) {
	    pali->pal[(216+i*6+j)*3] = 51*i;
	    pali->pal[(216+i*6+j)*3+1] = 212;
	    pali->pal[(216+i*6+j)*3+2] = 51*j;
	}
    } 
}

void
pal_2(struct pal_image* pali) {
    if(pali->pal != NULL) free(pali->pal);
    pali->pal = malloc(2*3*sizeof(unsigned char));
    pali->pal_len = 2;
    for(int i = 0 ; i < pali->pal_len ; i++) {
	pali->pal[i*3] = 255*i;
	pali->pal[i*3+1] = 255*i;
	pali->pal[i*3+2] = 255*i;
    }
}

void
pal_256(struct pal_image* pali) {
    if(pali->pal != NULL) free(pali->pal);
    pali->pal = malloc(256*3*sizeof(unsigned char));
    pali->pal_len = 256;
    for(int i = 0 ; i < pali->pal_len ; i++) {
	pali->pal[i*3] = 255*i;
	pali->pal[i*3+1] = 255*i;
	pali->pal[i*3+2] = 255*i;
    }
}

void
palette_dynamique ( struct pal_image *final , const struct image *initial , int n ) {
    if(final->pal != NULL) free(final->pal);
    final->pal = malloc ( 3*n*sizeof(unsigned char) ) ;
    if(final->pal == NULL) {
	fprintf(stderr, "Couldn't allocate pal_image.pal\n");
	return;
    }
    final->pal_len = n ;
    struct nb_couleur nb_palette[n];
    int nb_palette_len_max = n;
    for(int i = 0 ; i < nb_palette_len_max ; i++) {
	nb_palette[i].nb = 0;
	nb_palette[i].ucr = -1;
	nb_palette[i].ucv = -1;
	nb_palette[i].ucb = -1;
    }
    struct nb_couleur nc;
    struct node *abr = mknode(nb_palette[0], NULL, NULL);
  
    for(int i = 0 ; i < initial->height ; i++) {
	for(int j = 0 ; j < initial->width ; j++) {
	    nc.nb = 1 ;
	    nc.ucr = initial->data[i][ j*4     ] ;
	    nc.ucv = initial->data[i][ j*4 + 1 ] ;
	    nc.ucb = initial->data[i][ j*4 + 2 ] ;
	    insert ( nc , abr ) ;
	}
    }
    int nb_palette_len = 0;
    addTheMostCommonColors(abr, nb_palette, nb_palette_len_max, &nb_palette_len); 
    for(int i = 0 ; i < n ; i++) {
	final->pal[i*3  ] = nb_palette[i].ucr ;
	final->pal[i*3+1] = nb_palette[i].ucv ;
	final->pal[i*3+2] = nb_palette[i].ucb ;
    }
    free_tree(abr);
}

int
palette_dynamique_median_cut (struct pal_image* final, const struct image* initial, int palette_len ) {
    final->pal = malloc(palette_len*3*sizeof(unsigned char));
    final->pal_len = 0;
    unsigned char* colors = colors_tab_from_image(initial);
    if (colors == NULL) {
	fprintf(stderr, "Couldn't allocate an array of %d colors", initial->height*initial->width);
	return -1;
    }
    bucket(palette_len, final, colors, 0, initial->height*initial->width-1);
    free(colors);
    return 1;
}

int
findColorFromPalette(unsigned char color[3], struct pal_image* pali) {
    int k = 0;
    for(k = 0 ; k < pali->pal_len ; k++) {
	if (color[0] == pali->pal[k*3+0] &&
	    color[1] == pali->pal[k*3+1] &&
	    color[2] == pali->pal[k*3+2]) {
	    return k;
	}
    }
    return -1;
}

int
findClosestColorFromPalette(const unsigned char color[3], const struct pal_image* pali) {
    if (pali->pal_len <= 0) return -1;
    int minimal = normeEuclidienne(color, pali->pal), current = 0;
    int index = 0;
    for(int k = 0; k < pali->pal_len; k++) {
	current = normeEuclidienne(color, &pali->pal[k*3]);
	if(current <= minimal) {
	    index = k;
	    minimal = current;
	}			   
    }
    return index;
}

void
free_tree ( struct node *n ) {
    if ( n == NULL ) {
	return ;
    }
    free_tree ( n->left ) ;
    free_tree ( n->right ) ;
    free (n) ;    
}

struct node *
mknode ( struct nb_couleur nc , struct node *left , struct node *right ) {
    struct node *nouv = malloc ( sizeof(struct node) ) ;
    nouv->nc = nc ;
    nouv->left = left ;
    nouv->right = right ;
    return nouv ;
}

struct node *
insert(struct nb_couleur newnc, struct node *abr) {
    if(abr == NULL) {
	abr = mknode(newnc, NULL, NULL);
	return abr;
    }
    struct node* current = abr;
    while(1) {
	if(newnc.ucr < current->nc.ucr) {
	    if(current->left != NULL) {
		current = current->left;
	    } else {
		current->left = mknode(newnc, NULL, NULL);
		return current->left; 
	    }
	} else if(newnc.ucr > current->nc.ucr) {
	    if(current->right != NULL) {
		current = current->right;
	    } else {
		current->right = mknode(newnc, NULL, NULL);
		return current->right; 
	    }
	} else { // Si r1==r2 on passe au vert
	    if(newnc.ucv < current->nc.ucv) {
		if(current->left != NULL) {
		    current = current->left;
		} else {
		    current->left = mknode(newnc, NULL, NULL);
		    return current->left;
		}
	    } else if(newnc.ucv > current->nc.ucv) {
		if(current->right != NULL) {
		    current = current->right;
		} else {
		    current->right = mknode(newnc, NULL, NULL);
		    return current->right; 
		}
	    } else { // Si v1==v2 on passe au bleu
		if(newnc.ucb < current->nc.ucb) {
		    if(current->left != NULL) {
			current = current->left;
		    } else {
			current->left = mknode(newnc, NULL, NULL);
			return current->left; 
		    }
		} else if(newnc.ucb > current->nc.ucb) {
		    if(current->right != NULL) {
			current = current->right;
		    } else {
			current->right = mknode(newnc, NULL, NULL);
			return current->right; 
		    }
		} else {
		    current->nc.nb++; // Si rvb1==rvb2 on incrémente
		    return current;
		}
	    }
	}
    }
}

void
addTheMostCommonColors(struct node *tree, struct nb_couleur* nb_palette, int nb_palette_len_max, int* nb_palette_len) {
    if(!tree) return;
    if(tree->left)  addTheMostCommonColors(tree->left, nb_palette, nb_palette_len_max, nb_palette_len);
    if(tree->right) addTheMostCommonColors(tree->right, nb_palette, nb_palette_len_max, nb_palette_len);
    if (*nb_palette_len < nb_palette_len_max) {
	(*nb_palette_len)++;
	nb_palette[*nb_palette_len-1] = tree->nc;
	return;
    }
    for(int i = 0 ; i < nb_palette_len_max ; i++) {
        if(tree->nc.nb > nb_palette[i].nb) {
	    nb_palette[i] = tree->nc;	    
	    break;
	}
    }
}

void
bucket(int nb_bucket, struct pal_image* pali, unsigned char* colors, long int premier, long int dernier) {
    if (premier == dernier) {
	pali->pal_len++;
	pali->pal[(pali->pal_len-1)*3+0] = colors[0];
	pali->pal[(pali->pal_len-1)*3+1] = colors[1];
	pali->pal[(pali->pal_len-1)*3+2] = colors[2];
	return;
    }

    int color_reference = theOneWithTheMostAmplitude(colors, premier, dernier);
  
    quicksort(colors, premier, dernier, color_reference);
    
    if(nb_bucket > 1) {
	long int pivot_median = premier+(dernier-premier)/2;
	bucket(nb_bucket/2, pali, colors, premier, pivot_median);
	bucket(nb_bucket/2, pali, colors, pivot_median, dernier);
    } else {
	pali->pal_len++;
	unsigned char average_color[3];
	average_color_calculator(colors, premier, dernier, average_color);
	pali->pal[(pali->pal_len-1)*3+0] = average_color[0];
	pali->pal[(pali->pal_len-1)*3+1] = average_color[1];
	pali->pal[(pali->pal_len-1)*3+2] = average_color[2];
    }
}

int
theOneWithTheMostAmplitude(unsigned char* colors, long int premier, long int dernier) {
    unsigned char r_min, r_max, g_min, g_max, b_min, b_max, r_amplitude, g_amplitude, b_amplitude, max_amplitude;
    int color_reference = -1;
    r_min = colors[premier*3+0];
    r_max = colors[premier*3+0];
    g_min = colors[premier*3+1];
    g_max = colors[premier*3+1];
    b_min = colors[premier*3+2];
    b_max = colors[premier*3+2];
  
    for(long int i = premier ; i < dernier ; i++) {
	if(r_min > colors[i*3+0]) r_min = colors[i*3+0];
	if(g_min > colors[i*3+1]) g_min = colors[i*3+1];
	if(b_min > colors[i*3+2]) b_min = colors[i*3+2];
	if(r_max < colors[i*3+0]) r_max = colors[i*3+0];
	if(g_max < colors[i*3+1]) g_max = colors[i*3+1];
	if(b_max < colors[i*3+2]) b_max = colors[i*3+2];
    }

    r_amplitude = r_max - r_min;
    g_amplitude = g_max - g_min;
    b_amplitude = b_max - b_min;

    max_amplitude = max(r_amplitude, max(g_amplitude, b_amplitude));

    if(max_amplitude == r_amplitude) color_reference = 0;
    else if(max_amplitude == r_amplitude) color_reference = 1;
    else color_reference = 2;

    return color_reference;
}

void
average_color_calculator(unsigned char* colors, long int premier, long int dernier, unsigned char* average_color) {
    long double average_r = 0, average_g = 0, average_b = 0;
    long double coef;
    
    for(long int i = premier ; i <= dernier ; i++) {
	coef = i-premier+1;
	average_r = (average_r*(coef-1)+colors[i*3+0])/coef;
        average_g = (average_g*(coef-1)+colors[i*3+1])/coef;
        average_b = (average_b*(coef-1)+colors[i*3+2])/coef;
    }
    average_color[0] = (unsigned char)average_r;
    average_color[1] = (unsigned char)average_g;
    average_color[2] = (unsigned char)average_b;
}

void
quicksort(unsigned char* colors, long int premier, long int dernier, int color_reference) {
    long int pivot;
    if (premier < dernier) {
	pivot = choix_pivot(colors, premier, dernier);
	pivot = partitionner(colors, premier, dernier, pivot, color_reference);
	quicksort(colors, premier, pivot-1, color_reference);
	quicksort(colors, pivot+1, dernier, color_reference);
    }
}

void color_swapping(unsigned char* c1, unsigned char* c2) {
    unsigned char r = c1[0];
    unsigned char g = c1[1];
    unsigned char b = c1[2];
    c1[0] = c2[0];
    c1[1] = c2[1];
    c1[2] = c2[2];
    c2[0] = r;
    c2[1] = g;
    c2[2] = b;
}

long int
choix_pivot(unsigned char* colors, long int premier, long int dernier) {
    return premier+(dernier-premier)/2; //jsp
}

long int
partitionner(unsigned char* colors, long int premier, long int dernier, long int pivot, int color_reference) {
    color_swapping(&colors[pivot*3], &colors[dernier*3]);
    int j = premier;
    for(int i = premier ; i < dernier ; i++) {
	if(colors[i*3 + color_reference] <= colors[dernier*3 + color_reference]) {
	    color_swapping(&colors[i*3], &colors[j*3]);
	    j++;
	}
    }
    color_swapping(&colors[dernier*3], &colors[j*3]);
    return j;
}

int
normeEuclidienne(const unsigned char* C1, const unsigned char* C2) {
    return (C1[0]-C2[0])*(C1[0]-C2[0]) +  (C1[1]-C2[1])*(C1[1]-C2[1]) + (C1[2]-C2[2])*(C1[2]-C2[2]);
}

unsigned char*
colors_tab_from_image(const struct image* img) {
    long int colors_len = img->height*img->width, n = 0;
    unsigned char* colors = malloc(3*colors_len*sizeof(unsigned char));
    if(colors == NULL) {
	fprintf(stderr, "Can't allocate an array of %ld colors\n", colors_len);
	return NULL;
    }
    for(int i = 0 ; i < img->height ; i++) {
	for(int j = 0 ; j < img->width ; j++) {
	    colors[n*3+0] = img->data[i][j*4+0];
	    colors[n*3+1] = img->data[i][j*4+1];
	    colors[n*3+2] = img->data[i][j*4+2];
	    n++;
	}
    }
    return colors;
}

unsigned char
max(unsigned char c1, unsigned char c2) {
    return (c1 > c2) ? c1 : c2;
}

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "palette.h"

struct nb_couleur {
    int nb;
    unsigned char ucr, ucv, ucb;
} ;

struct node {
  struct nb_couleur nc ;
  struct node *left ;
  struct node *right ;
} ;

void free_tree ( struct node *n );
struct node * mknode ( struct nb_couleur nc , struct node *left , struct node *right );
struct node * insert(struct nb_couleur newnc, struct node *abr);
void addTheMostCommonColors(struct node *tree, struct nb_couleur* nb_palette, int nb_palette_len);
void color_swapping(unsigned char* c1, unsigned char* c2);
long int choix_pivot(unsigned char* colors, long int premier, long int dernier);
long int partitionner(unsigned char* colors, long int premier, long int dernier, long int pivot, int color_reference);
void quicksort(unsigned char* colors, long int premier, long int dernier, int color_reference);
unsigned char* colors_tab_from_image(struct image* img);
unsigned char max(unsigned char c1, unsigned char c2);
void bucket(int nb_bucket, struct pal_image* pali, unsigned char* colors, long int premier, long int dernier);
void average_color_calculator(unsigned char* colors, long int premier, long int dernier, unsigned char* average_color);

void
pal_8(struct pal_image* pali) {
    //rajouter un free au cas où pali->pal != NULL ?
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
    pali->pal = malloc(256*3*sizeof(unsigned char));
    pali->pal_len = 256;
    for(int i = 0 ; i < pali->pal_len ; i++) {
	pali->pal[i*3] = 255*i;
	pali->pal[i*3+1] = 255*i;
	pali->pal[i*3+2] = 255*i;
    }
}

unsigned char
findClosestColorFromPalette(const unsigned char* originalPixel, const unsigned char* palette, const int pal_len) {
    int minimal = normeEuclidienne(originalPixel, palette), current = 0;
    unsigned char index = -1;
    for(int k = 0; k < pal_len; k++) {
	current = normeEuclidienne(originalPixel, &palette[k*3]);
	if(current <= minimal) {
	    index = k;
	    minimal = current;
	}			   
    }
    
    return index;
}

int
normeEuclidienne(const unsigned char* C1, const unsigned char* C2) {
    return (C1[0]-C2[0])*(C1[0]-C2[0]) +  (C1[1]-C2[1])*(C1[1]-C2[1]) + (C1[2]-C2[2])*(C1[2]-C2[2]);
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
	} else { // Si r=r on passe au vert
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
	    } else { // Si v=v on passe au bleu
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
		    current->nc.nb++; // Si rvb=rvb on incrémente
		    return current;
		}
	    }
	}
    }
}

void
addTheMostCommonColors(struct node *tree, struct nb_couleur* nb_palette, int nb_palette_len)
{
    if(!tree) return;

    if(tree->left)  addTheMostCommonColors(tree->left, nb_palette, nb_palette_len);

    for(int i = 0 ; i < nb_palette_len ; i++) {
	if(tree->nc.nb > nb_palette[i].nb) {
	    nb_palette[i] = tree->nc;	    
	    break;
	}
    }  

    if(tree->right) addTheMostCommonColors(tree->right, nb_palette, nb_palette_len);
}

unsigned char
max(unsigned char c1, unsigned char c2) {
    return (c1 > c2) ? c1 : c2;
}

void
palette_dynamique ( struct pal_image *final , struct image *initial , int n ) {
  final->pal = malloc ( 3*n*sizeof(unsigned char) ) ;
  if(final->pal == NULL) {
      fprintf(stderr, "Couldn't allocate pal_image.pal\n");
      return;
  }
  final->pal_len = n ;
  struct nb_couleur nb_palette[n];
  int nb_palette_len = n;
  for(int i = 0 ; i < nb_palette_len ; i++) {
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

  addTheMostCommonColors(abr, nb_palette, nb_palette_len); 

  for(int i = 0 ; i < n ; i++) {
      final->pal[i*3  ] = nb_palette[i].ucr ;
      final->pal[i*3+1] = nb_palette[i].ucv ;
      final->pal[i*3+2] = nb_palette[i].ucb ;
  }

  free_tree(abr);
}

void
palette_dynamique_median_cut(struct image* initial, struct pal_image* final, int palette_len) {
    final->pal = malloc(palette_len*3*sizeof(unsigned char));
    final->pal_len = 0;
    unsigned char* colors = colors_tab_from_image(initial);

    bucket(palette_len, final, colors, 0, initial->height*initial->width-1);

    free(colors);
}

void
bucket(int nb_bucket, struct pal_image* pali, unsigned char* colors, long int premier, long int dernier) {

    unsigned char r_min, r_max, g_min, g_max, b_min, b_max, r_amplitude, g_amplitude, b_amplitude, max_amplitude;
    int color_reference = -1;
    r_min = colors[premier*3+0];
    r_max = colors[premier*3+0];
    g_min = colors[premier*3+1];
    g_max = colors[premier*3+1];
    b_min = colors[premier*3+2];
    b_max = colors[premier*3+2];
  
    for(long int i = premier ; i < dernier ; i++) {
	//printf("%ld : (%d,%d,%d)\n",i ,colors[i*3+0],colors[i*3+1],colors[i*3+2]);
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

    /* printf("r_amplitude = %d\n", r_amplitude); */
    /* printf("g_amplitude = %d\n", g_amplitude); */
    /* printf("b_amplitude = %d\n", b_amplitude); */

    max_amplitude = max(r_amplitude, max(g_amplitude, b_amplitude));

    if(max_amplitude == r_amplitude) color_reference = 0;
    else if(max_amplitude == r_amplitude) color_reference = 1;
    else color_reference = 2;
  
    quicksort(colors, premier, dernier, color_reference);

    /* for(int i = premier ; i < 10 ; i++) { */
    /* 	printf("(%d,%d,%d)\n", colors[i*3+0], colors[i*3+1], colors[i*3+2]); */
    /* } */
    /* for(int i = dernier-10 ; i < dernier ; i++) { */
    /* 	printf("(%d,%d,%d)\n", colors[i*3+0], colors[i*3+1], colors[i*3+2]); */
    /* } */

    //nb_bucket = sqrt(nb_bucket);
    if(nb_bucket > 1) {
	long int pivot_median = premier+(dernier-premier)/2;
	bucket(nb_bucket/2, pali, colors, premier, pivot_median);
	bucket(nb_bucket/2, pali, colors, pivot_median, dernier);
    } else {
	pali->pal_len++;
	unsigned char average_color[3];
	average_color_calculator(colors, premier, dernier, average_color);
	//printf("%d : (%d,%d,%d)\n", pali->pal_len, average_color[0], average_color[2], average_color[2]);
	pali->pal[(pali->pal_len-1)*3+0] = average_color[0];
	pali->pal[(pali->pal_len-1)*3+1] = average_color[1];
	pali->pal[(pali->pal_len-1)*3+2] = average_color[2];
    }
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

unsigned char*
colors_tab_from_image(struct image* img) {
    long int colors_len = img->height*img->width, n = 0;
    unsigned char* colors = malloc(3*colors_len*sizeof(unsigned char));
    if(colors == NULL) {
	fprintf(stderr, "Aïe aïe aïe... l'image est trop grande.");
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

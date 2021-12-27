#include <png.h>
#include <stdio.h>
#include <stdlib.h>

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
    int minimal = norme(originalPixel, palette), current = 0;
    unsigned char index = -1;
    for(int k = 0; k < pal_len; k++) {
	current = norme(originalPixel, &palette[k*3]);
	if(current <= minimal) {
	    index = k;
	    minimal = current;
	}			   
    }
    
    return index;
}

int
norme(const unsigned char* C1, const unsigned char* C2) {
    return (C1[0]-C2[0])*(C1[0]-C2[0]) + (C1[1]-C2[1])*(C1[1]-C2[1]) + (C1[2]-C2[2])*(C1[2]-C2[2]);
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
  struct node *abr = mknode(nb_palette[0], NULL, NULL), *newabr;
  struct nb_couleur* pt_nb_couleur_min = &nb_palette[0];
  
  for(int i = 0 ; i < initial->height ; i++) {
      for(int j = 0 ; j < initial->width ; j++) {
	  nc.nb = 1 ;
	  nc.ucr = initial->data[i][ j*4     ] ;
	  nc.ucv = initial->data[i][ j*4 + 1 ] ;
	  nc.ucb = initial->data[i][ j*4 + 2 ] ;
	  newabr = insert ( nc , abr ) ;
	  for(int l = 0 ; l < nb_palette_len ; l++) {
	      if(newabr->nc.ucr == nb_palette[l].ucr &&
		 newabr->nc.ucv == nb_palette[l].ucv &&
		 newabr->nc.ucb == nb_palette[l].ucb) {
		  nb_palette[l].nb = newabr->nc.nb;
		  pt_nb_couleur_min = &nb_palette[l];
		  break;
	      } else if(pt_nb_couleur_min->nb > nb_palette[l].nb) {
		  pt_nb_couleur_min = &nb_palette[l];
	      }
	  }
	  if(newabr->nc.nb > pt_nb_couleur_min->nb) {
	      *pt_nb_couleur_min = newabr->nc ;
	  }
      }
  }

  for(int i = 0 ; i < n ; i++) {
      final->pal[i*3  ] = nb_palette[i].ucr ;
      final->pal[i*3+1] = nb_palette[i].ucv ;
      final->pal[i*3+2] = nb_palette[i].ucb ;
  }

  free_tree(abr);
}

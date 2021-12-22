#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#include "palette.h"

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
    unsigned char index = 0 ;
    for(int k = 0; k < pal_len; k++) {
	current = norme(originalPixel, &palette[k*3]) ;
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

struct nb_couleur {
  unsigned char ir , iv , ib ;
  int nb ;
} ;

struct node {
  struct nb_couleur *nc ;
  struct node *left ;
  struct node *right ;
} ;

struct nb_couleur *init_cl () {
  struct nb_couleur *nc = malloc ( sizeof(struct nb_couleur) ) ;
  nc->nb = 1 ;
  return nc ;
}

struct node *mknode ( struct nb_couleur *nc , struct node *left , struct node *right ) {
  struct node *nouv = malloc ( sizeof(struct node) ) ;
  nouv->nc = nc ;
  nouv->left = left ;
  nouv->right = right ;
  return nouv ;
}

void free_tree ( struct node *n ) {
  if ( n == NULL ) {
    return ;
  }
  free_tree ( n->left ) ;
  free_tree ( n->right ) ;
  free (n) ;    
}

// VOIR SI JE PEUX PAS LE FAIRE PLUS COURT
struct node *insert ( struct nb_couleur *nc , struct node *abr , int n ) { // n=1 : ir ; n=2 : iv ; n=3 : ib , n=-1 : nombre , TJS METTRE n=1 ou -1
  if ( n==1 ) { // ROUGE
    if ( abr == NULL ) {
      return mknode( nc , NULL , NULL ) ;
    }
    if ( nc->ir < abr->nc->ir ) {
      if ( abr->left == NULL ) {
	abr->left = mknode( nc , NULL , NULL ) ;
      } else {
	insert ( nc , abr->left , 1 ) ;
      }
    } else if ( nc->ir > abr->nc->ir ) {
      if ( abr->right == NULL ) {
	abr->right = mknode( nc , NULL , NULL ) ;
      } else {
	insert ( nc , abr->right , 1 ) ;
      }
    } else if ( nc->ir == abr->nc->ir ) {
      insert ( nc , abr , 2 ) ; // REGARDE LE VERT
    }
  } else if ( n==2 ) { // VERT
    if ( abr == NULL ) {
      return mknode( nc , NULL , NULL ) ;
    }
    if ( nc->iv < abr->nc->iv ) {
      if ( abr->left == NULL ) {
	abr->left = mknode( nc , NULL , NULL ) ;
      } else {
	insert ( nc , abr->left , 2 ) ;
      }
    } else if ( nc->iv > abr->nc->iv ) {
      if ( abr->right == NULL ) {
	abr->right = mknode( nc , NULL , NULL ) ;
      } else {
	insert ( nc , abr->right , 2 ) ;
      }
    } else if ( nc->iv == abr->nc->iv ) {
      insert ( nc , abr , 3 ) ; // REGARDE LE BLEU
    }
  } else if ( n==3 ) { // BLEU
    if ( abr == NULL ) {
      return mknode( nc , NULL , NULL ) ;
    }
    if ( nc->ib < abr->nc->ib ) {
      if ( abr->left == NULL ) {
	abr->left = mknode( nc , NULL , NULL ) ;
      } else {
	insert ( nc , abr->left , 3 ) ;
      }
    } else if ( nc->ib > abr->nc->ib ) {
      if ( abr->right == NULL ) {
	abr->right = mknode( nc , NULL , NULL ) ;
      } else {
	insert ( nc , abr->right , 3 ) ;
      }
    } else if ( nc->ib == abr->nc->ib ) {
      abr->nc->nb++ ; // MODIFIE LE NOMBRE
    }
  } else if ( n==-1 ) {
    if ( abr == NULL ) {
      return mknode( nc , NULL , NULL ) ;
    }
    if ( nc->nb <= abr->nc->nb ) {
      if ( abr->left == NULL ) {
	abr->left = mknode( nc , NULL , NULL ) ;
      } else {
	insert ( nc , abr->left , -1 ) ;
      }
    } else {
      if ( abr->right == NULL ) {
	abr->right = mknode( nc , NULL , NULL ) ;
      } else {
	insert ( nc , abr->right , -1 ) ;
      }
    }
  }
  return abr ;
}

struct buffer {
  struct nb_couleur **c ;
  int len ;
  int cap ;
} ;

struct buffer *new_buffer ( int cap ) {
  struct nb_couleur **c = malloc ( cap*sizeof(struct nb_couleur) ) ;
  struct buffer *b = malloc ( sizeof(struct buffer) ) ;
  b->len = 0 ;
  b->cap = cap ;
  b->c = c ;
  return b ;
}

void destroy_buffer ( struct buffer *b ) {
  free ( b->c ) ;
  free ( b ) ;
}

int snoc ( struct buffer *b , struct nb_couleur *c ) {
  if ( b->len == b->cap ) { // PB ICI => PAS UN PB PEUT ETRE
    return -1 ;
  } else {
    b->c[b->len] = c ;
    b->len++ ;
    //printf("len=%d\nnb=%d\nr=%c\nv=%c\nb=%c\n", b->len, c->nb , c->ir, c->iv, c->ib) ; // SORT RIEN APRES LE R V ET B
    return 1 ;
  }
}

void compare ( struct nb_couleur *nc , struct buffer *b , int n ) {
  if ( b->len < n ) { // FONCTIONNE
    snoc ( b , nc ) ;
  } else {  // APRES IL FAUT COMPARER LES NC RESSTANTES
    for ( int i = 0 ; i < n ; i++ ) {
      //printf("boucle for\n");
      //printf("b->c[%d]->nb=%d", i,  b->c[i]->nb); // LE PROBLEME EST : b->c[%d]->nb
      if ( nc->nb > b->c[i]->nb ) { 
	//printf("test");
	b->c[i] = nc ;
	//printf("i=%d\n", i );
	break ;
      }
    }
  }
}
  
// LOGIQUEMENT FONCTIONNE / POURTANT PB LIGNE 334
void recup_infix_max ( struct node *abr , struct buffer *b , int n ) {
    if ( abr == NULL ) {
      //printf("0\n");
	return ;
    }
    recup_infix_max ( abr->left , b , n ) ;
    compare ( abr->nc , b , n ) ;
    recup_infix_max ( abr->right , b , n ) ;
}

void palette_dynamique ( struct pal_image *final , struct image *initial , int n ) {
  final->pal = malloc ( 3*n*sizeof(unsigned char) ) ;
  final->pal_len = n ;
  // ARBRE COULEUR
  struct nb_couleur *nc = init_cl () ;
  nc->ir = initial->data[0][0] ;
  nc->iv = initial->data[0][1] ;
  nc->ib = initial->data[0][2] ;
  struct node *abr = mknode ( nc , NULL , NULL ) ;
  //printf("ir=%c\niv=%c\nib=%c\n\n fin\n\n", nc->ir , nc->iv , nc->ib);
  for(int i = 0 ; i < initial->height ; i++) {
    for(int j = 0 ; j < initial->width ; j++) {
      if ( i!=0 && j!=0 ) {
	struct nb_couleur *nc2 = init_cl () ;
	nc2->ir = initial->data[i][ j*4 ] ;
	nc2->iv = initial->data[i][ j*4 + 1 ] ;
	nc2->ib = initial->data[i][ j*4 + 2 ] ;
	/*	if ( i==j ) {
		printf("ir=%c\niv=%c\nib=%c\n\n", nc2->ir , nc2->iv , nc2->ib);
		}*/
	insert ( nc2 , abr , 1 ) ;
      }
    }
  }
  //printf("1ere boucle finie\n");
  // NOMBRE
  struct buffer *tab = new_buffer ( n*2 ) ;
  //printf("test\n"); // JUSQU'ICI, CA SORT
  recup_infix_max ( abr , tab , n ) ; // PROBLEME ICIIIIII
  //printf("test2\n");
  // IMAGE FINALE
  for ( int i = 0 ; i < tab->len ; i++ ) {
    final->pal[ i*3 ] = tab->c[i]->ir ;
    final->pal[ i*3 + 1 ] = tab->c[i]->iv ;
    final->pal[ i*3 +2 ] = tab->c[i]->ib ;
    printf("r=%d\n" ,final->pal[ i*3]) ;
    printf("v=%d\n" ,final->pal[ i*3 + 1 ]) ;
    printf("b=%d\n\n" ,final->pal[ i*3 +2 ]) ;
  }
  destroy_buffer ( tab ) ;
}

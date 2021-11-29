#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#include "conversion.h"

int max_abs (int a , int b ) ;
int norme_a_la_puissance ( int num_norme , const struct image *initial , const struct pal_image *final , int k_pali , int i_data , int j_data ) ;


int
gen_pal_image(struct pal_image* pali, const struct image* img , int num_norme ) {
    int exist, min, min_i, current;
    if(pali->pal_len == -1 || pali->pal == NULL) {
	pali->pal = calloc(3*256, 3*256*sizeof(unsigned char));
	pali->pal_len = 0;
	for(int i = 0; i < img->height; i++) {
	    for(int j = 0; j < img->width; j++) {
		exist = 0;
		for(int k = 0; k < pali->pal_len; k++) {
		    if(img->data[i][j*4] == pali->pal[k*3]
		       && img->data[i][j*4+1] == pali->pal[k*3+1]
		       && img->data[i][j*4+2] == pali->pal[k*3+2]) {
			pali->data[i][j] = k;
			exist = 1;
			break;
		    }
		}
		if(exist == 0 && pali->pal_len < 256) {
		    pali->pal_len++;
		    pali->pal[pali->pal_len*3-3] = img->data[i][j*4];
		    pali->pal[pali->pal_len*3-2] = img->data[i][j*4+1];
		    pali->pal[pali->pal_len*3-1] = img->data[i][j*4+2];
		    pali->data[i][j] = pali->pal_len-1;
		} else if (exist == 0 && pali->pal_len >= 256) {
			free( pali->pal ) ;
			pali->pal_len = -1 ;
		    return -1;
		}
	    }
	}
    } else {
	for(int i = 0; i < img->height; i++) {
	    for(int j = 0; j < img->width; j++) {
		min = norme_a_la_puissance ( num_norme , img , pali , 0 , i , j ) ;
		min_i = 0 ;
		current = 0;
		for(int k = 0; k < pali->pal_len; k++) {
		    current = norme_a_la_puissance ( num_norme , img , pali , k , i , j ) ;
		    if(current <= min) {
			min_i = k;
			min = current;
		    }			   
		}					
		pali->data[i][j] = min_i;
	    }
	}
    }
    return 1;
}

int
floydSteinberg(struct pal_image* pali, struct image* img , int num_norme ) {
    int exist, min, min_i, current;
    char newPixel[3], oldPixel[3], errorPixel[3];
    if(pali->pal_len == -1 || pali->pal == NULL) {
	pali->pal = calloc(3*256, 3*256*sizeof(char));
	pali->pal_len = 0;
	for(int i = 0; i < img->height; i++) {
	    for(int j = 0; j < img->width; j++) {
		exist = 0;
		for(int k = 0; k < pali->pal_len; k++) {
		    if(img->data[i][j*4] == pali->pal[k*3]
		       && img->data[i][j*4+1] == pali->pal[k*3+1]
		       && img->data[i][j*4+2] == pali->pal[k*3+2]) {
			pali->data[i][j] = k;
			exist = 1;
			break;
		    }
		}
		if(exist == 0 && pali->pal_len < 256) {
		    pali->pal_len++;
		    pali->pal[pali->pal_len*3-3] = img->data[i][j*4];
		    pali->pal[pali->pal_len*3-2] = img->data[i][j*4+1];
		    pali->pal[pali->pal_len*3-1] = img->data[i][j*4+2];
		    pali->data[i][j] = pali->pal_len-1;
		} else if (exist == 0 && pali->pal_len >= 256) {
		    return -1;
		}
	    }
	}
    } else {
	for(int i = 0; i < img->height; i++) {
	    for(int j = 0; j < img->width; j++) {
		for(int l = 0 ; l < 3 ; l++) { // l=0 -> Red ; l=1 -> Green ; l=2 -> Blue ; 
		    oldPixel[l] = img->data[i][j*4+l];
		}
		min = norme_a_la_puissance ( num_norme , img , pali , 0 , i , j ) ;
		min_i = 0 ;
		current = 0;
		for(int k = 0; k < pali->pal_len; k++) {
		    current = norme_a_la_puissance ( num_norme , img , pali , k , i , j ) ;
		    if(current <= min) {
			min_i = k;
			min = current;
		    }			   
		}
		pali->data[i][j] = min_i;
		for(int l = 0 ; l < 3 ; l++) {
		    newPixel[l] = pali->pal[min_i*3+l];
		    errorPixel[l] = oldPixel[l] - newPixel[l];
		}
		int p;
		for(int l = 0 ; l < 3 ; l++) { //pixel est	        
		    if(j*4+4 < img->width) {
			p = img->data[i  ][j*4+4+l] + 7.0/16*errorPixel[l];
			if(p < 0)
			    img->data[i  ][j*4+4+l] = 0;
			else if(p > 255)
			    img->data[i  ][j*4+4+l] = 255;
			else
			    img->data[i  ][j*4+4+l] = p;
		    }
		    if(i+1 < img->height && j*4-4 >= 0) {  //pixel sud-ouest
			p = img->data[i+1][j*4-4+l] + 3.0/16*errorPixel[l];
			if(p < 0)
			    img->data[i+1][j*4-4+l] = 0;
			else if(p > 255)
			    img->data[i+1][j*4-4+l] = 255;
			else
			    img->data[i+1][j*4-4+l] = p;
			    
		    }
		    if(i+1 < img->height) { //pixel sud
			p = img->data[i+1][j*4  +l] + 5.0/16*errorPixel[l];
			if(p < 0)
			    img->data[i+1][j*4  +l] = 0;
			else if(p > 255)
			    img->data[i+1][j*4  +l] = 255;
			else
			    img->data[i+1][j*4  +l] = p;
		    }
		    if(i+1 < img->height && j*4+4 < img->width) { //pixel sud-est
			p = img->data[i+1][j*4+4+l] + 1.0/16*errorPixel[l];
			if(p < 0)
			    img->data[i+1][j*4+4+l] = 0;
			else if(p > 255)
			    img->data[i+1][j*4+4+l] = 255;
			else
			    img->data[i+1][j*4+4+l] = p;
		    }
		}
	    }
	}
    }
    return 1;
}


int max_abs (int a , int b ) {
  if ( a < 0 ) {
    a = -a ;
  }
  if ( b < 0 ) {
    b = -b ;
  }
  if ( a > b ) {
    return a ;
  } else {
    return b ;
  }
}

/* 0 pour la norme infinie */

int norme_a_la_puissance ( int num_norme , const struct image *initial , const struct pal_image *final , int k_pali , int i_data , int j_data ) {
  int somme = 0 ;
  if ( k_pali < final->pal_len && i_data < initial->height && j_data < initial->width && k_pali >= 0 && i_data >= 0 && j_data >= 0 ) {
    if ( num_norme != 0 ) {
      for ( int rgb = 0 ; rgb < 3 ; rgb++ ) {
	int produit = 1 ;
	for ( int p = 0 ; p < num_norme ; p++ ) {
	  produit *= final->pal[k_pali*3+rgb] - initial->data[i_data][j_data*4+rgb] ;
	}
	if ( produit < 0 ) {
	  produit = - produit ;
	}
	somme += produit ;
      }
    } else {
      return max_abs( max_abs( final->pal[k_pali*3] - initial->data[i_data][j_data*4] , final->pal[k_pali*3+1] - initial->data[i_data][j_data*4+1] ) , final->pal[k_pali*3+2] - initial->data[i_data][j_data*4+2] ) ;
    }
  } else {
    return -1 ;
  }
  return somme ;
}

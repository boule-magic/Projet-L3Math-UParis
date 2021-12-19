#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#include "palette.h"
#include "conversion.h"

int max_abs (int a , int b ) ;
int norme_a_la_puissance ( int num_norme , const struct image *initial , const struct pal_image *final , int k_pali , int i_data , int j_data ) ;
struct image* new_scaled_image(double factor, const struct image* img);
void errorPixelCalcul(unsigned char* originalPixel, unsigned char* newPixel, int* errorPixel);
void errorApplication(unsigned char* pixel, int* errorPixel, double coef);
unsigned char uCharCap(int num);

struct pal_image*
new_pal_image(const struct image* img) {
    struct pal_image* pali = calloc(1, sizeof(struct pal_image));
    if(pali == NULL) {
        fprintf(stderr, "Couldn't allocate pal_image.\n");
        return NULL;
    }
    pali->width = img->width;
    pali->height = img->height;
    pali->data = malloc(img->height * sizeof(unsigned char*));
    if(pali->data == NULL) {
        fprintf(stderr, "Couldn't allocate data pal.\n");
	free(pali);
        return NULL;
    }
    //pali->pal = NULL;
    pali->pal_len = -1;
    for(int i = 0; i < img->height; i++) {
        pali->data[i] = malloc(img->width);
        if(pali->data[i] == NULL) {
            fprintf(stderr, "Couldn't allocate data pal.\n");
            for ( int k = 0 ; k < i ; k++ ) {
            	free( pali->data[k] ) ;
            }
	    free(pali->data);
	    free(pali);
            return NULL;
        }
    }
    return pali;
}

int
gen_pal_image(struct pal_image* pali, struct image* img , int num_norme ) {
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

////////////////////////////// Dithering

int
floydSteinberg(struct pal_image* pali, struct image* img , int num_norme ) {
    unsigned char newPixel[3], originalPixel[3], index;
    int errorPixel[3];
    if(pali->pal == NULL) {
        return -1;
    }
    for(int i = 0; i < img->height; i++) {
	for(int j = 0; j < img->width; j++) {
	    originalPixel[0] = img->data[i][j*4  ];
	    originalPixel[1] = img->data[i][j*4+1];
	    originalPixel[2] = img->data[i][j*4+2];
		
	    index = findClosestColorFromPalette(originalPixel, pali->pal, pali->pal_len);
	    pali->data[i][j] = index;
		
	    newPixel[0] = pali->pal[index*3  ];
	    newPixel[1] = pali->pal[index*3+1];
	    newPixel[2] = pali->pal[index*3+2];
		
	    errorPixelCalcul(originalPixel, newPixel, errorPixel);
		
	    if(j+1 < img->width) errorApplication(&img->data[i][j*4+4], errorPixel, 7.0/16); //pixel est
	    if(i+1 < img->height && j-1 >= 0) errorApplication(&img->data[i+1][j*4-4], errorPixel, 3.0/16); //pixel sud-ouest
	    if(i+1 < img->height) errorApplication(&img->data[i+1][j*4], errorPixel, 5.0/16); //pixel sud
	    if(i+1 < img->height && j+1 < img->width) errorApplication(&img->data[i+1][j*4+4], errorPixel, 1.0/16);//pixel sud-est
	}
    }

    return 1;
}

int
atkinson(struct pal_image* pali, struct image* img , int num_norme ) {
    unsigned char newPixel[3], originalPixel[3], index;
    int errorPixel[3];
    if(pali->pal == NULL) {
        return -1;
    }
    for(int i = 0; i < img->height; i++) {
	for(int j = 0; j < img->width; j++) {
	    originalPixel[0] = img->data[i][j*4  ];
	    originalPixel[1] = img->data[i][j*4+1];
	    originalPixel[2] = img->data[i][j*4+2];
		
	    index = findClosestColorFromPalette(originalPixel, pali->pal, pali->pal_len);
	    pali->data[i][j] = index;
		
	    newPixel[0] = pali->pal[index*3  ];
	    newPixel[1] = pali->pal[index*3+1];
	    newPixel[2] = pali->pal[index*3+2];
		
	    errorPixelCalcul(originalPixel, newPixel, errorPixel);
		
	    if(j+1 < img->width) errorApplication(&img->data[i][j*4+4], errorPixel, 1.0/8); //pixel est
	    if(j+2 < img->width) errorApplication(&img->data[i][j*4+8], errorPixel, 1.0/8); //pixel est est
	    if(i+1 < img->height && j-1 >= 0) errorApplication(&img->data[i+1][j*4-4], errorPixel, 1.0/8); //pixel sud-ouest
	    if(i+1 < img->height) errorApplication(&img->data[i+1][j*4], errorPixel, 1.0/8); //pixel sud
	    if(i+1 < img->height && j+1 < img->width) errorApplication(&img->data[i+1][j*4+4], errorPixel, 1.0/8);//pixel sud-est
	    if(i+2 < img->height) errorApplication(&img->data[i+2][j*4], errorPixel, 1.0/8); //pixel sud sud
	}
    }
	
    return 1;
}

void
errorPixelCalcul(unsigned char* originalPixel, unsigned char* newPixel, int* errorPixel)
{

    errorPixel[0] = originalPixel[0] - newPixel[0];
    errorPixel[1] = originalPixel[1] - newPixel[1];
    errorPixel[2] = originalPixel[2] - newPixel[2];
}

void
errorApplication(unsigned char* pixel, int* errorPixel, double coef)
{
    pixel[0] = uCharCap(pixel[0] + (errorPixel[0] * coef));
    pixel[1] = uCharCap(pixel[1] + (errorPixel[1] * coef));
    pixel[2] = uCharCap(pixel[2] + (errorPixel[2] * coef));
}

unsigned char
uCharCap(int num)
{
    if (num > 255) return 255;
    if (num < 0) return 0;

    return num;
}

/////////////////////////// scaling

struct image*
new_scaled_image(double factor, const struct image* img) {
    struct image* smallimg = calloc(1, sizeof(struct pal_image));
    if(smallimg == NULL) {
        fprintf(stderr, "Couldn't allocate pal_image.\n");
        return NULL;
    }
    smallimg->width = img->width/factor;
    smallimg->height = img->height/factor;
    smallimg->data = malloc(smallimg->height * sizeof(unsigned char*));
    if(smallimg->data == NULL) {
        fprintf(stderr, "Couldn't allocate data pal.\n");
	free(smallimg);
        return NULL;
    }
    for(int i = 0; i < smallimg->height; i++) {
        smallimg->data[i] = malloc(4*smallimg->width);
        if(smallimg->data[i] == NULL) {
            fprintf(stderr, "Couldn't allocate data pal.\n");
            for ( int k = 0 ; k < i ; k++ ) {
            	free( smallimg->data[k] ) ;
            }
	    free(smallimg->data);
	    free(smallimg);
            return NULL;
        }
    }
    return smallimg;
}

struct image*
image_scaling(double factor, const struct image* img) {
    struct image* smallimg = new_scaled_image(factor, img);
    if(smallimg == NULL) {
	return NULL;
    }
    for(int i = 0 ; i < smallimg->height ; i++) {
	for(int j = 0 ; j < smallimg->width ; j++) {
	    int fi = i*factor;
	    int fj = j*factor;
	    /* double ffi = i*factor - fi; */
	    /* double ffj = j*factor - fj; */
	    /* if(fi < img->height + 1 && fj < img->width + 1) { // bilinear interpolation (ou presque) crado */
	    /* 	smallimg->data[i][j*4  ] =  (1-ffi)*img->data[fi][fj*4  ] + (ffi)*img->data[fi][fj*4+4  ]; */
	    /* 	smallimg->data[i][j*4+1] =  (1-ffi)*img->data[fi][fj*4+1] + (ffi)*img->data[fi][fj*4+4+1]; */
	    /* 	smallimg->data[i][j*4+2] =  (1-ffi)*img->data[fi][fj*4+2] + (ffi)*img->data[fi][fj*4+4+2]; */
	    /* 	smallimg->data[i][j*4+3] =  (1-ffi)*img->data[fi][fj*4+3] + (ffi)*img->data[fi][fj*4+4+3]; */
	    /* } else { */
		smallimg->data[i][j*4  ] =  img->data[fi][fj*4  ];
		smallimg->data[i][j*4+1] =  img->data[fi][fj*4+1];
		smallimg->data[i][j*4+2] =  img->data[fi][fj*4+2];
		smallimg->data[i][j*4+3] =  img->data[fi][fj*4+3];
	    /* } */
	}
    }
    return smallimg;
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





#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "palette.h"
#include "dithering.h"

//int max_abs (int a , int b ) ;
//int norme_a_la_puissance ( int num_norme , const struct image *initial , const struct pal_image *final , int k_pali , int i_data , int j_data ) ;
void errorPixelCalcul(unsigned char* originalPixel, unsigned char* newPixel, int* errorPixel);
void errorApplication(unsigned char* pixel, int* errorPixel, double coef);
unsigned char uCharCap(int num);

const int BAYER_16X16[16][16] =	{ // 16x16 Bayer Dithering Matrix.  Color levels: 256
    {	  0, 191,  48, 239,  12, 203,  60, 251,   3, 194,  51, 242,  15, 206,  63, 254	}, 
    {	127,  64, 175, 112, 139,  76, 187, 124, 130,  67, 178, 115, 142,  79, 190, 127	},
    {	 32, 223,  16, 207,  44, 235,  28, 219,  35, 226,  19, 210,  47, 238,  31, 222	},
    {	159,  96, 143,  80, 171, 108, 155,  92, 162,  99, 146,  83, 174, 111, 158,  95	},
    {	  8, 199,  56, 247,   4, 195,  52, 243,  11, 202,  59, 250,   7, 198,  55, 246	},
    {	135,  72, 183, 120, 131,  68, 179, 116, 138,  75, 186, 123, 134,  71, 182, 119	},
    {	 40, 231,  24, 215,  36, 227,  20, 211,  43, 234,  27, 218,  39, 230,  23, 214	},
    {	167, 104, 151,  88, 163, 100, 147,  84, 170, 107, 154,  91, 166, 103, 150,  87	},
    {	  2, 193,  50, 241,  14, 205,  62, 253,   1, 192,  49, 240,  13, 204,  61, 252	},
    {	129,  66, 177, 114, 141,  78, 189, 126, 128,  65, 176, 113, 140,  77, 188, 125	},
    {	 34, 225,  18, 209,  46, 237,  30, 221,  33, 224,  17, 208,  45, 236,  29, 220	},
    {	161,  98, 145,  82, 173, 110, 157,  94, 160,  97, 144,  81, 172, 109, 156,  93	},
    {	 10, 201,  58, 249,   6, 197,  54, 245,   9, 200,  57, 248,   5, 196,  53, 244	},
    {	137,  74, 185, 122, 133,  70, 181, 118, 136,  73, 184, 121, 132,  69, 180, 117	},
    {	 42, 233,  26, 217,  38, 229,  22, 213,  41, 232,  25, 216,  37, 228,  21, 212	},
    {	169, 106, 153,  90, 165, 102, 149,  86, 168, 105, 152,  89, 164, 101, 148,  85	}
};

int
naive_pal_image(struct pal_image* pali, const struct image* img) {
    int index;
    if(pali->pal != NULL) {
	for(int i = 0; i < img->height; i++) {
	    for(int j = 0; j < img->width; j++) {					
		pali->data[i][j] = findClosestColorFromPalette(&img->data[i][j*4], pali);
	    }
	}
    } else {
	pali->pal = calloc(3*256, 3*256*sizeof(unsigned char));
	pali->pal_len = 0;
	for(int i = 0; i < img->height; i++) {
	    for(int j = 0; j < img->width; j++) {
		index = findColorFromPalette(&img->data[i][j*4], pali);
		if (index != -1) {
		    pali->data[i][j] = index;
		} else if (pali->pal_len < 256) {
		    pali->pal_len++;
		    pali->pal[pali->pal_len*3-3] = img->data[i][j*4];
		    pali->pal[pali->pal_len*3-2] = img->data[i][j*4+1];
		    pali->pal[pali->pal_len*3-1] = img->data[i][j*4+2];
		    pali->data[i][j] = pali->pal_len-1;
		} else {
		    fprintf(stderr, "Too many colors.\n");
		    fprintf(stderr, "You should choose a static/dynamic palette\n");
		    pali->pal_len = 0 ;
		    return -1;
		}
		
	    }
	}
    }
    return 1;
}

int
floydSteinberg_pal_image(struct pal_image* pali, struct image* img) {
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
		
	    index = findClosestColorFromPalette(originalPixel, pali);
	    pali->data[i][j] = index;
		
	    newPixel[0] = pali->pal[index*3  ];
	    newPixel[1] = pali->pal[index*3+1];
	    newPixel[2] = pali->pal[index*3+2];
		
	    errorPixelCalcul(originalPixel, newPixel, errorPixel);
		
	    if(j+1 < img->width) errorApplication(&img->data[i][j*4+4], errorPixel, 7.0/16); //pixel est
	    if(i+1 < img->height && j-1 >= 0) errorApplication(&img->data[i+1][j*4-4], errorPixel, 3.0/16); //pixel sud-ouest
	    if(i+1 < img->height) errorApplication(&img->data[i+1][j*4], errorPixel, 5.0/16); //pixel sud
	    if(i+1 < img->height && j+1 < img->width) errorApplication(&img->data[i+1][j*4+4], errorPixel, 1.0/16); //pixel sud-est
	}
    }

    return 1;
}

// -d 2

int
atkinson_pal_image(struct pal_image* pali, struct image* img) {
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
		
	    index = findClosestColorFromPalette(originalPixel, pali);
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

int
ordered_pal_image_static(struct pal_image* pali, const struct image* img) {
    if(pali->pal == NULL) {
        return -1;
    }
    switch(pali->pal_len) {
    case 2:
	ordered_pal_image_nb_rgb(pali,img,1,1,1);
	break;
    case 8:
	ordered_pal_image_nb_rgb(pali,img,1,1,1);
	break;
    case 16:
	ordered_pal_image_nb_rgb(pali,img,2,2,2);
	break;
    case 64:
	ordered_pal_image_nb_rgb(pali,img,3,3,3);
	break;
    case 216:
	ordered_pal_image_nb_rgb(pali,img,5,5,5);
	break;
    case 252:
	ordered_pal_image_nb_rgb(pali,img,5,6,5);
	break;	
    }
    return 1;
}

int
ordered_pal_image_nb_rgb(struct pal_image* pali, const struct image* img, int nb_red, int nb_green, int nb_blue) {
    // (r,g,b) ∈ ([0,nb_red],[0,nb_green],[0,nb_blue])
    unsigned char pixel[3];
    int  corr[3];
    int divider[3] = {256/nb_red, 256/nb_green, 256/nb_blue} ;
    for(int i = 0 ; i < pali->height ; i++)
	{
	    for(int j = 0 ; j < pali->width ; j++)
		{						      
		    pixel[0] = img->data[i][j * 4 + 0];
		    pixel[1] = img->data[i][j * 4 + 1];
		    pixel[2] = img->data[i][j * 4 + 2];
		    
		    corr[0] = BAYER_16X16[i%16][j%16] / nb_red;
		    corr[1] = BAYER_16X16[i%16][j%16] / nb_green;
		    corr[2] = BAYER_16X16[i%16][j%16] / nb_blue;

		    int	a = (pixel[0] + corr[0]) / divider[0];
		    a = a < nb_red ? a : nb_red;
		    a = a * divider[0] < 255 ? a * divider[0] : 255;
		    
		    int	b = (pixel[1] + corr[1]) / divider[1];
		    b = b < nb_green ? b : nb_green;
		    b = b * divider[1] < 255 ? b * divider[1] : 255;
		    
		    int	c = (pixel[2] + corr[2]) / divider[2];
		    c = c < nb_blue ? c : nb_blue;
		    c = c * divider[2] < 255 ? c * divider[2] : 255;

		    pixel[0] = a;
		    pixel[1] = b;
		    pixel[2] = c;
		    
		    pali->data[i][j] = findClosestColorFromPalette(pixel, pali);
		}
	}
    return 1;
}

/* void */
/* ordered_pal_image_216(struct pal_image* pali, const struct image* img) { */
/*     unsigned char pixel[3]; */
/*     int  corr[3]; */
/*     int  ncolors = 5; */
/*     int divider = 256 / ncolors; */
/*     for(int i = 0 ; i < pali->height ; i++) */
/* 	{ */
/* 	    for(int j = 0 ; j < pali->width ; j++) */
/* 		{						       */
/* 		    pixel[0] = img->data[i][j * 4 + 0]; */
/* 		    pixel[1] = img->data[i][j * 4 + 1]; */
/* 		    pixel[2] = img->data[i][j * 4 + 2]; */
		    
/* 		    corr[0] = BAYER_16X16[i%16][j%16] / ncolors; */
/* 		    corr[1] = BAYER_16X16[i%16][j%16] / ncolors; */
/* 		    corr[2] = BAYER_16X16[i%16][j%16] / ncolors; */

/* 		    int	i1	= (pixel[0] + corr[0]) / divider; */
/* 		    i1 = i1 < ncolors ? i1 : ncolors; */
/* 		    i1 = i1 * divider < 255 ? i1 * divider : 255; */
/* 		    int	i2	= (pixel[1] + corr[1]) / divider; */
/* 		    i2 = i2 < ncolors ? i2 : ncolors; */
/* 		    i2 = i2 * divider < 255 ? i2 * divider : 255; */
/* 		    int	i3	= (pixel[2] + corr[2]) / divider; */
/* 		    i3 = i3 < ncolors ? i3 : ncolors; */
/* 		    i3 = i3 * divider < 255 ? i3 * divider : 255; */

/* 		    pixel[0] = i1; */
/* 		    pixel[1] = i2; */
/* 		    pixel[2] = i3; */
		    
/* 		    pali->data[i][j] = findClosestColorFromPalette(pixel, pali); */
/* 		} */
/* 	} */
/* } */

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


/* int max_abs (int a , int b ) { */
/*   if ( a < 0 ) { */
/*     a = -a ; */
/*   } */
/*   if ( b < 0 ) { */
/*     b = -b ; */
/*   } */
/*   if ( a > b ) { */
/*     return a ; */
/*   } else { */
/*     return b ; */
/*   } */
/* } */

/* /\* 0 pour la norme infinie *\/ */

/* int norme_a_la_puissance ( int num_norme , const struct image *initial , const struct pal_image *final , int k_pali , int i_data , int j_data ) { */
/*     int somme = 0 ; */
/*     if ( k_pali < final->pal_len && i_data < initial->height && j_data < initial->width && k_pali >= 0 && i_data >= 0 && j_data >= 0 ) { */
/* 	if ( num_norme != 0 ) { */
/* 	    for ( int rgb = 0 ; rgb < 3 ; rgb++ ) { */
/* 		int produit = 1 ; */
/* 		for ( int p = 0 ; p < num_norme ; p++ ) { */
/* 		    produit *= final->pal[k_pali*3+rgb] - initial->data[i_data][j_data*4+rgb] ; */
/* 		} */
/* 		if ( produit < 0 ) { */
/* 		    produit = - produit ; */
/* 		} */
/* 		somme += produit ; */
/* 	    } */
/* 	} else { */
/* 	    return max_abs( max_abs( final->pal[k_pali*3] - initial->data[i_data][j_data*4] , final->pal[k_pali*3+1] - initial->data[i_data][j_data*4+1] ) , final->pal[k_pali*3+2] - initial->data[i_data][j_data*4+2] ) ; */
/* 	} */
/*     } else { */
/* 	return -1 ; */
/*     } */
/*     return somme ; */
/* } */





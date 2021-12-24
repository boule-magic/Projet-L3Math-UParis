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
const	double BAYER_2X2[2][2]  =  {{-0.25, 0.25},{0.5, 0}};        //	2x2 Bayer Dithering Matrix. Color levels: 5 ou 4 ? Pré-calculée !!
const	int BAYER_4X4[4][4]  =  {                       //	4x4 Bayer Dithering Matrix. Color levels: 17
    {	 15, 195,  60, 240	},
    {	135,  75, 180, 120	},
    {	 45, 225,  30, 210	},
    {	165, 105, 150,  90	}

};
const	int BAYER_8X8[8][8]		=	{	//	8x8 Bayer Dithering Matrix. Color levels: 65
    {	  0, 128,  32, 160,   8, 136,  40, 168	},
    {	192,  64, 224,  96, 200,  72, 232, 104	},
    {	 48, 176,  16, 144,  56, 184,  24, 152	},
    {	240, 112, 208,  80, 248, 120, 216,  88	},
    {	 12, 140,  44, 172,   4, 132,  36, 164	},
    {	204,  76, 236, 108, 196,  68, 228, 100	},
    {	 60, 188,  28, 156,  52, 180,  20, 148	},
    {	252, 124, 220,  92, 244, 116, 212,  84	}
};
const	int	BAYER_16X16[16][16]	=	{	//	16x16 Bayer Dithering Matrix.  Color levels: 256
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
    int exist;
    if(pali->pal_len == -1 || pali->pal == NULL) {
	pali->pal = calloc(3*256, 3*256*sizeof(unsigned char));
	pali->pal_len = 0;
	for(int i = 0; i < img->height; i++) {
	    for(int j = 0; j < img->width; j++) {
		exist = findClosestColorFromPalette(&img->data[i][j*4], pali->pal, pali->pal_len);
		if(exist == -1 && pali->pal_len < 256) {
		    pali->pal_len++;
		    pali->pal[pali->pal_len*3-3] = img->data[i][j*4];
		    pali->pal[pali->pal_len*3-2] = img->data[i][j*4+1];
		    pali->pal[pali->pal_len*3-1] = img->data[i][j*4+2];
		    pali->data[i][j] = pali->pal_len-1;
		} else if (exist == -1 && pali->pal_len >= 256) {
			free( pali->pal ) ;
			pali->pal_len = -1 ;
		    return -1;
		}
	    }
	}
    } else {
	for(int i = 0; i < img->height; i++) {
	    for(int j = 0; j < img->width; j++) {					
		pali->data[i][j] = findClosestColorFromPalette(&img->data[i][j*4], pali->pal, pali->pal_len);
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


int
ordered_pal_image(struct pal_image* pali, const struct image* img) {
    unsigned char pixel[3];
    int  diff[3]; 
    for(int i = 0 ; i < pali->height ; i++)
	{
	    for(int j = 0 ; j < pali->width ; j++)
		{
		    pixel[0] = img->data[i][j * 4 + 0];
		    pixel[1] = img->data[i][j * 4 + 1];
		    pixel[2] = img->data[i][j * 4 + 2];
		    
		    diff[0] = BAYER_16X16[i%16][j%16];
		    diff[1] = BAYER_16X16[i%16][j%16];
		    diff[2] = BAYER_16X16[i%16][j%16];
		    
		    errorApplication(pixel, diff, 1.0/cbrt(pali->pal_len));
		    
		    pali->data[i][j] = findClosestColorFromPalette(pixel, pali->pal, pali->pal_len); 
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





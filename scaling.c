#include <stdio.h>
#include <stdlib.h>

#include "scaling.h"


struct image*
new_scaled_image(double factor, const struct image* img) {
    struct image* scaledimg = calloc(1, sizeof(struct pal_image));
    if(scaledimg == NULL) {
        fprintf(stderr, "Couldn't allocate scaledimg.\n");
        return NULL;
    }
    scaledimg->width = img->width*factor;
    scaledimg->height = img->height*factor;
    scaledimg->data = malloc(scaledimg->height * sizeof(unsigned char*));
    if(scaledimg->data == NULL) {
        fprintf(stderr, "Couldn't allocate scaledimg's data.\n");
	free(scaledimg);
        return NULL;
    }
    for(int i = 0; i < scaledimg->height; i++) {
        scaledimg->data[i] = malloc(4*scaledimg->width);
        if(scaledimg->data[i] == NULL) {
            fprintf(stderr, "Couldn't allocate scaledimg's data.\n");
            for ( int k = 0 ; k < i ; k++ ) {
            	free( scaledimg->data[k] ) ;
            }
	    free(scaledimg->data);
	    free(scaledimg);
            return NULL;
        }
    }
    return scaledimg;
}

struct image*
image_scaling(int height, int width, const struct image* img) {
    double factor = 0;
    if (height != 0 && width != 0) {
	fprintf(stderr, "Let's not distort the image please.\n");
	return NULL;
    } else if(height < 0 || width < 0) {
	fprintf(stderr, "Input error.\n");
	return NULL;
    } else if (height > 0 && width == 0) {
	factor = height / (double) img->height; 
    } else if (width > 0 && height == 0) {
	factor = width / (double) img->width; 
    } else {
	fprintf(stderr, "Impossible error.");
	return NULL;
    }
    
    printf("factor = %.2f\n", factor);
    struct image* scaledimg = new_scaled_image(factor, img);
    if(scaledimg == NULL) {
	return NULL;
    }
    for(int i = 0 ; i < scaledimg->height ; i++) {
	for(int j = 0 ; j < scaledimg->width ; j++) {
	    int fi = i/factor;
	    int fj = j/factor;
	    double i_err = i/factor - fi;
	    double j_err = j/factor - fj;
	    
	    if(fi + 1 < img->height && fj + 1 < img->width) { // bilinear interpolation (ou presque)
		scaledimg->data[i][j*4  ] =  (1-j_err)*img->data[fi][fj*4]       + (j_err)*img->data[fi  ][fj*4+4  ];
		scaledimg->data[i][j*4  ] =  (1-i_err)*scaledimg->data[i][j*4]   + (i_err)*img->data[fi+1][fj*4    ];
		scaledimg->data[i][j*4+1] =  (1-j_err)*img->data[fi][fj*4+1]     + (j_err)*img->data[fi  ][fj*4+4+1];
		scaledimg->data[i][j*4+1] =  (1-i_err)*scaledimg->data[i][j*4+1] + (i_err)*img->data[fi+1][fj*4+1  ];
		scaledimg->data[i][j*4+2] =  (1-j_err)*img->data[fi][fj*4+2]     + (j_err)*img->data[fi  ][fj*4+4+2];
		scaledimg->data[i][j*4+2] =  (1-i_err)*scaledimg->data[i][j*4+2] + (i_err)*img->data[fi+1][fj*4+2  ];
		scaledimg->data[i][j*4+3] =  (1-j_err)*img->data[fi][fj*4+3]     + (j_err)*img->data[fi  ][fj*4+4+3];
		scaledimg->data[i][j*4+3] =  (1-i_err)*scaledimg->data[i][j*4+3] + (i_err)*img->data[fi+1][fj*4+3  ];
	    } /* else { */
	    /* 	scaledimg->data[i][j*4  ] =  img->data[fi][fj*4  ]; */
	    /* 	scaledimg->data[i][j*4+1] =  img->data[fi][fj*4+1]; */
	    /* 	scaledimg->data[i][j*4+2] =  img->data[fi][fj*4+2]; */
	    /* 	scaledimg->data[i][j*4+3] =  img->data[fi][fj*4+3]; */
	    /* } */
	}
    }
    return scaledimg;
}

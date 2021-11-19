#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pngio.h"

int
main(int argc, char **argv)
{
    struct image *image, *image2;
    int rc;

    while(1) {
        int opt;

        opt = getopt(argc, argv, "");
        if(opt < 0)
            break;

        switch(opt) {
        default:
            fprintf(stderr, "Unknown option.\n");
            return 1;
        }
    }

    if(argc != optind + 2) {
        fprintf(stderr, "Requires 2 parameters.\n");
        return 1;
    }

    image = read_png(argv[optind]);
    if(image == NULL) {
        fprintf(stderr, "Couldn't read %s\n", argv[optind]);
        return 1;
    }

    image2 = calloc(1, sizeof(struct image));
    if(image2 == NULL) {
        fprintf(stderr, "Couldn't allocate image2.\n");
        return 1;
    }

    image2->width = image->width;
    image2->height = image->height;
    image2->data = malloc(image->height * sizeof(unsigned char*));
    if(image2->data == NULL) {
        fprintf(stderr, "Couldn't allocate data 2.\n");
        return 1;
    }
    for(int i = 0; i < image->height; i++) {
        image2->data[i] = malloc(4 * image->width);
        if(image2->data[i] == NULL) {
            fprintf(stderr, "Couldn't allocate data 2.\n");
            return 1;
        }
    }

    for(int i = 0; i < image->height; i++) {
        for(int j = 0; j < image->width; j++) {
                image2->data[i][j * 4] =
                    image->data[i][j * 4];
                image2->data[i][j * 4 + 1] =
                    image->data[i][j * 4+ 1];
                image2->data[i][j * 4 + 2] =
                    image->data[i][j * 4 + 2];
                image2->data[i][j * 4 + 3] =
                    image->data[i][j * 4 + 3];
            }
    }

    //Moué début
    pal_image* pali = calloc(1, sizeof(pal_image));
    if(image2 == NULL) {
        fprintf(stderr, "Couldn't allocate pal_image.\n");
        return 1;
    }
    pali->width = image->width;
    pali->height = image->height;
    pali->data = malloc(image->height * sizeof(unsigned char*));
    if(pali->data == NULL) {
        fprintf(stderr, "Couldn't allocate data pal.\n");
        return 1;
    }
    pali->pal = calloc(255, 255*3*sizeof(unsigned char));
    pali->pal_len = 0;
    for(int i = 0; i < image->height; i++) {
        pali->data[i] = malloc(image->width);
        if(pali->data[i] == NULL) {
            fprintf(stderr, "Couldn't allocate data pal.\n");
            return 1;
        }
    }
    int indice = -1;
    for(int i = 0; i < image->height; i++) {
        for(int j = 0; j < image->width; j++) {
	    for(int k = 0; k < pali->pal_len; k++) {
		indice = -1;
		if(pali->pal[k*3] == image->data[i][j*4]/40*40
		   && pali->pal[k*3+1] == image->data[i][j*4+1]/40*40
		   && pali->pal[k*3+2] == image->data[i][j*4+2]/40*40) {
		    indice = k;
		    break;
		}
	    }
	    if(indice == -1) {
	        pali->pal_len += 1;
	        pali->pal[pali->pal_len*3-3] = image->data[i][j*4]/40*40;
		pali->pal[pali->pal_len*3-2] = image->data[i][j*4+1]/40*40;
		pali->pal[pali->pal_len*3-1] = image->data[i][j*4+2]/40*40;
		pali->data[i][j] = pali->pal_len-1;
	    }
	    else {
                pali->data[i][j] = indice;
            }
	}
    }
    //Moué fin

    rc = write_png(argv[optind + 1], image);
    if(rc < 0) {
        fprintf(stderr, "Couldn't write %s.\n", argv[optind + 1]);
        return 1;
    }

    rc = write_pal_png(argv[optind + 1], pali);
    if(rc < 0) {
        fprintf(stderr, "Couldn't write %s.\n", argv[optind + 1]);
        return 1;
    }

    free_image(image);
    free_image(image2);
    free_pal_image(pali);

    return 0;
}



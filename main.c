#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pngio.h"

int
main(int argc, char **argv)
{
    struct image *image;
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

    pal_image* pali = calloc(1, sizeof(pal_image));
    if(pali == NULL) {
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

    rc = write_pal_png(argv[optind + 1], pali);
    if(rc < 0) {
        fprintf(stderr, "Couldn't write %s.\n", argv[optind + 1]);
        return 1;
    }

    free_image(image);
    free_pal_image(pali);

    return 0;
}



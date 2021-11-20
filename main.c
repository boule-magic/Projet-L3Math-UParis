#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "pngio.h"

int
main(int argc, char **argv)
{
    struct image *img;
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

    if(argc != optind + 1 && argc != optind + 2) {
        fprintf(stderr, "Requires 1 or 2 parameters.\n");
        return 1;
    }

    img = read_png(argv[optind]);
    if(img == NULL) {
        fprintf(stderr, "Couldn't read %s\n", argv[optind]);
        return 1;
    }

    pal_image* pali = new_pal_image(img);
    int indice = -1;
    for(int i = 0; i < img->height; i++) {
        for(int j = 0; j < img->width; j++) {
	    for(int k = 0; k < pali->pal_len; k++) {
		indice = -1;
		if(pali->pal[k*3] == img->data[i][j*4]/40*40
		   && pali->pal[k*3+1] == img->data[i][j*4+1]/40*40
		   && pali->pal[k*3+2] == img->data[i][j*4+2]/40*40) {
		    indice = k;
		    break;
		}
	    }
	    if(indice == -1) {
	        pali->pal_len += 1;
	        pali->pal[pali->pal_len*3-3] = img->data[i][j*4]/40*40;
		pali->pal[pali->pal_len*3-2] = img->data[i][j*4+1]/40*40;
		pali->pal[pali->pal_len*3-1] = img->data[i][j*4+2]/40*40;
		pali->data[i][j] = pali->pal_len-1;
	    }
	    else {
                pali->data[i][j] = indice;
            }
	}
    }

    if(argv[optind + 1] == NULL) {
        rc = write_pal_png("./img/output.png", pali);
	if(rc < 0) {
	    fprintf(stderr, "Couldn't write ./img/output.png.\n");
	    return 1;
	}
    }
    else {
	rc = write_pal_png(argv[optind + 1], pali);
	if(rc < 0) {
	    fprintf(stderr, "Couldn't write %s.\n", argv[optind + 1]);
	    return 1;
	}
    }

    

    free_image(img);
    free_pal_image(pali);

    system("eog img/output.png");

    return 0;
}



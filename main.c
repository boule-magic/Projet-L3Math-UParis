#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "pngio.h"

pal_image* new_pal_image(const image* img);
void pal_8(pal_image* pali); //definition d'une palette de 8 couleurs (saturation)
void pal_16(pal_image* pali);
void pal_64(pal_image* pali);
void pal_216(pal_image* pali);
void pal_252(pal_image* pali);
void pal_2(pal_image* pali);
void pal_256(pal_image* pali);
void gen_pal_image(pal_image* pali, const image* img);
pal_image* generation1(const image* img); //bricole à supprimer



int
main(int argc, char **argv)
{
    image* img;
    pal_image* pali;
    int rc;
    char option = 0;

    //lecture des paramètres du main (arc,argv)
    while(1) {
        int opt;

        opt = getopt(argc, argv, "abcdefg"); //"ab::c:" argless a, optarg b, mandatoryarg c
        if(opt < 0)
            break;
	
        switch(opt) {
	case 'a':
	    option = 'a';
	    break;
	case 'b':
	    option = 'b';
	    break;
	    case 'c':
	    option = 'c';
	    break;
	case 'd':
	    option = 'd';
	    break;
	    case 'e':
	    option = 'e';
	    break;
	case 'f':
	    option = 'f';
	    break;
	    case 'g':
	    option = 'g';
	    break;
        default:
            fprintf(stderr, "Unknown option %c.\n", opt);
            return 1;
        }
    }

    if(argc != optind + 1 && argc != optind + 2) {
        fprintf(stderr, "Requires 1 or 2 parameters.\n");
        return 1;
    }
    
    //lecture de l'image source et mise en mémoire
    img = read_png(argv[optind]);
    if(img == NULL) {
        fprintf(stderr, "Couldn't read %s\n", argv[optind]);
        return 1;
    }

    //création de l'image à palette de couleur
    pali = new_pal_image(img); //allocation
    if(pali == NULL) {
	fprintf(stderr, "Arf pali broken !\n");
        return 1;
    }
    switch(option) {
    case 'a':
	printf("Palette de 8 couleurs : saturation\n");
	pal_8(pali); //définition palette de 8 couleurs
	gen_pal_image(pali, img); //génération de l'image à palette de couleurs
	break;
    case 'b':
	printf("Palette de 16 couleurs : CGA\n");
	pal_16(pali);
	gen_pal_image(pali, img);
	break;
    case 'c':
	printf("Palette de 64 couleurs : 4-4-4\n");
	pal_64(pali);
	gen_pal_image(pali, img);
	break;
    case 'd':
	printf("Palette de 216 couleurs : 6-6-6\n");
	pal_216(pali);
	gen_pal_image(pali, img);
	break;
    case 'e':
	printf("Palette de 252 couleurs : 6-7-6\n");
	pal_252(pali);
	gen_pal_image(pali, img);
	break;
    case 'f':
	printf("Palette de 2 couleurs : noir et blanc\n");
	pal_2(pali);
	gen_pal_image(pali, img);
	break;
    case 'g':
	printf("Palette de 256 couleurs : niveaux de gris\n");
	pal_256(pali);
	gen_pal_image(pali, img);
	break;
    default:
	pal_252(pali);
	gen_pal_image(pali, img);
	break;
    }

    //écriture de l'image générée
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
    
    //free
    free_image(img);
    free_pal_image(pali);

    //ouverture automatique de l'image générée
    if(argv[optind + 1] == NULL) {
	system("eog img/output.png");
    }
    else {
	char cmd[strlen(argv[optind + 1]) + 5];
	strcpy(cmd, "eog ");
	strcat(cmd, argv[optind + 1]);	
        system(cmd);
    }

    return 0;
}

pal_image*
new_pal_image(const image* img) {
    pal_image* pali = calloc(1, sizeof(pal_image));
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
	    free(pali->data);
	    free(pali);
            return NULL;
        }
    }
    return pali;
}

void
pal_8(pal_image* pali) {
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
pal_16(pal_image* pali) {
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
pal_64(pal_image* pali) {
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
pal_216(pal_image* pali) {
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
pal_252(pal_image* pali) {
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
pal_2(pal_image* pali) {
    pali->pal = malloc(2*3*sizeof(unsigned char));
    pali->pal_len = 2;
    for(int i = 0 ; i < pali->pal_len ; i++) {
	pali->pal[i*3] = 255*i;
	pali->pal[i*3+1] = 255*i;
	pali->pal[i*3+2] = 255*i;
    }
}

void
pal_256(pal_image* pali) {
    pali->pal = malloc(256*3*sizeof(unsigned char));
    pali->pal_len = 256;
    for(int i = 0 ; i < pali->pal_len ; i++) {
	pali->pal[i*3] = 255*i;
	pali->pal[i*3+1] = 255*i;
	pali->pal[i*3+2] = 255*i;
    }
}

void
gen_pal_image(pal_image* pali, const image* img) {
    int current=0, min=0, min_i = -1;
    for(int i = 0; i < img->height; i++) {
        for(int j = 0; j < img->width; j++) {
	    min = 3*255*255;
	    current = 0;
	    for(int k = 0; k < pali->pal_len; k++) {
		current = (pali->pal[k*3]-img->data[i][j*4])*(pali->pal[k*3]-img->data[i][j*4])
		    + (pali->pal[k*3+1]-img->data[i][j*4+1])*(pali->pal[k*3+1]-img->data[i][j*4+1])
		    + (pali->pal[k*3+2]-img->data[i][j*4+2])*(pali->pal[k*3+2]-img->data[i][j*4+2]);
		if(current <= min) {
		    min_i = k;
		    min = current;
		}			   
	    }					
	    pali->data[i][j] = min_i;
	}
    }
}

pal_image* generation1(const image* img) {
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
    return pali;
}

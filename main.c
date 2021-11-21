#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "palette.h"

pal_image* new_pal_image(const image* img);


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


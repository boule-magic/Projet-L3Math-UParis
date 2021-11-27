#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "palette.h"

struct pal_image* new_pal_image(const struct image* img);


int
main(int argc, char **argv)
{
    struct image* img;
    struct pal_image* pali;
    int rc;
    char option = 0;
    int argument;

    //lecture des paramètres du main (arc,argv)
    while(1) {
        int opt;

        opt = getopt(argc, argv, "p:"); //"ab::c:" argless a, optarg b, mandatoryarg c
        if(opt < 0)
            break;
	
        switch(opt) {
	case 'p': // "p" comme "palette"
	    option = opt;
	    argument = atoi(optarg);
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
    case 'p':
        switch(argument) {
        case 8:
	    printf("Palette de 8 couleurs : saturation\n");
	    pal_8(pali); //définition palette de 8 couleurs
	    gen_pal_image(pali, img); //génération de l'image à palette de couleurs
	    break;
	case 16:
	    printf("Palette de 16 couleurs : CGA\n");
	    pal_16(pali);
	    gen_pal_image(pali, img);
	    break;
	case 64:
	    printf("Palette de 64 couleurs : 4-4-4\n");
	    pal_64(pali);
	    gen_pal_image(pali, img);
	    break;
	case 216:
	    printf("Palette de 216 couleurs : 6-6-6\n");
	    pal_216(pali);
	    gen_pal_image(pali, img);
	    break;
	case 252:
	    printf("Palette de 252 couleurs : 6-7-6\n");
	    pal_252(pali);
	    gen_pal_image(pali, img);
	    break;
	case 2:
	    printf("Palette de 2 couleurs : noir et blanc\n");
	    pal_2(pali);
	    gen_pal_image(pali, img);
	    break;
	case 256:
	    printf("Palette de 256 couleurs : niveaux de gris\n");
	    pal_256(pali);
	    gen_pal_image(pali, img);
	    break;
	default:
	    printf("Unavailable palette\n");
	    printf("The available palettes are :\n");
	    printf("Colors :\n");
	    printf("-p 8\n");
	    printf("-p 16\n");
	    printf("-p 64\n");
	    printf("-p 216\n");
	    printf("-p 252\n");
	    printf("Black & white :\n");
	    printf("-p 2\n");
	    printf("-p 256\n");
	    return 1;
	}
	break;
    default:
        printf("Usage: ./a.out [options] file...\n");
	printf("Options:\n");
	printf("  -p <arg>    Select the number of colors in the palette:\n");
	return 1;
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
	    free(pali->data);
	    free(pali);
            return NULL;
        }
    }
    return pali;
}


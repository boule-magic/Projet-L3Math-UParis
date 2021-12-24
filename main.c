#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "palette.h"
#include "dithering.h"
#include "scaling.h"

int
main(int argc, char **argv)
{
    struct image* img;
    struct pal_image* pali;
    int rc;
    int argp = 0, argd = 0, argx = 0, argw = 0, argh = 0;
    
    //lecture des paramètres du main (arc,argv)
    while(1) {
        int opt;

        opt = getopt(argc, argv, "xd:p:w:h:"); //"ab::c:" argless a, optarg b, mandatoryarg c
        if(opt < 0)
            break;
	
        switch(opt) {
	case 'p': // "p" comme "palette"
	    if(optarg != NULL) 	argp = atoi(optarg);
	    else argp = 0;
	    break;
	case 'd': // "d" comme "dithering" et "diffusion d'erreur"
	    if(optarg != NULL) argd = atoi(optarg) ;
	    else argd = 0 ;
	    break;
	case 'h' : // "h" comme "height"
	    if(optarg != NULL) argh = atoi(optarg);
	    else argh = 0;
	    break;
	case 'w' : // "w" comme "width"
	    if(optarg != NULL) argw = atoi(optarg);
	    else argw = 0;
	    break;
	case 'x': // "x" comme "execution"
	    argx = 1;
	    break;
        default:
            fprintf(stderr, "Usage: %s [source.png] [output.png] [-p number] [-d 1/2/3] [-n number] [-x] [-w width] [-h height] [-s scaling_factor]\n", argv[0]);
            return 1;
        }
    }

    if(argc != optind + 1 && argc != optind + 2) {
        fprintf(stderr, "Usage: %s [source.png] [output.png] [-p number] [-d 1/2/3] [-n number] [-x] [-w width] [-h height] [-s scaling_factor]\n", argv[0]);
        return 1;
    }
    
    //lecture de l'image source et mise en mémoire
    img = read_png(argv[optind]);
    if(img == NULL) {
        fprintf(stderr, "Couldn't read %s\n", argv[optind]);
        return 1;
    }

    //redimensionnement
    if(argh > 0 || argw > 0) {
	struct image* smallimg = image_scaling(argh, argw, img);
	if(smallimg == NULL) {
	    fprintf(stderr, "Couldn't scale\n");
	} else {
	    free_image(img);
	    img = smallimg;
	}
    }

    //création de l'image à palette de couleur
    pali = new_pal_image(img); //allocation
    if(pali == NULL) {
	fprintf(stderr, "pal_image is broken !\n");
        return 1;
    }

    //création de la palette de couleur
    switch(argp) {
        case 8:
	    printf("Palette de 8 couleurs : saturation\n");
	    pal_8(pali); //définition palette de 8 couleurs
	    break;
	case 16:
	    printf("Palette de 16 couleurs : CGA\n");
	    pal_16(pali);
	    break;
	case 64:
	    printf("Palette de 64 couleurs : 4-4-4\n");
	    pal_64(pali);
	    break;
	case 216:
	    printf("Palette de 216 couleurs : 6-6-6\n");
	    pal_216(pali);
	    break;
	case 252:
	    printf("Palette de 252 couleurs : 6-7-6\n");
	    pal_252(pali);
	    break;
	case 2:
	    printf("Palette de 2 couleurs : noir et blanc\n");
	    pal_2(pali);
	    break;
	case 256:
	    printf("Palette de 256 couleurs : niveaux de gris\n");
	    pal_256(pali);
	    break;
        case 0:
	    printf("Unavailable palette\n");
	default:
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

    ///création de l'image indexée
    switch(argd) {
    case 0:
	printf("Conversion en image indexée classique\n");
	if(naive_pal_image(pali, img) == -1) {
	    fprintf(stderr, "Conversion error\n");
	    return 1;
	}
	break;
    case 1:
	printf("Conversion en image indexée + dispersion d'erreur de Floyd-Steinberg\n");
	if(floydSteinberg_pal_image(pali, img) == -1) {
	    fprintf(stderr, "Conversion error\n");
	    return 1;
	}
	break;
    case 2:
	printf("Conversion en image indexée + dispersion d'erreur d'Atkinson\n");
	if(atkinson_pal_image(pali, img) == -1) {
	    fprintf(stderr, "Conversion error\n");
	    return 1;
	}
	break;
    case 3:
	printf("Conversion en image indexée + tramage ordonné\n");
	if(ordered_pal_image(pali, img) == -1) {
	    fprintf(stderr, "Conversion error\n");
	    return 1;
	}
	break;
    default:
	fprintf(stderr, "Invalid argd\n");
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
    if(argx == 1) { 
	if(argv[optind + 1] == NULL) {
	    char cmd[] = "eog img/output.png";
	    int syst = system(cmd);
	    if ( syst != 0 ) {
		fprintf( stderr, "Unable to launch command : %s\n", cmd );
		return 1;
	    }
	}
	else {
	    char cmd[strlen(argv[optind + 1]) + 5];
	    strcpy(cmd, "eog ");
	    strcat(cmd, argv[optind + 1]);	
	    int syst = system(cmd);
	    if ( syst != 0 ) {
		fprintf( stderr, "Unable to launch command  : %s\n", cmd );
		return 1;
	    }
	}
    }

    return 0;
}


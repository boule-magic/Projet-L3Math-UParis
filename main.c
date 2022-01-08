#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "palette.h"
#include "dithering.h"
#include "scaling.h"
#include "colorspace.h"

void usage(FILE *f, char **argv);
void fast_automatic(struct pal_image* pali, struct image* img) {
    printf("Conversion en image indexée\n");
    if(naive_pal_image(pali, img) == -1) {
	fprintf(stderr, "Conversion error\n");
	free(pali->pal);
	if(img->height*img->width < 500000) {
	    palette_dynamique_median_cut(pali, img, 256);
	    floydSteinberg_pal_image(pali, img);
	} else {
	    palette_252(pali);
	    atkinson_pal_image(pali, img);
	}
    }
}

int
main(int argc, char **argv)
{
    struct image* img;
    struct pal_image* pali;
    int rc;
    int argp = 0, argd = 0, argx = 0, argw = 0, argh = 0, argP = 0, argf = 0, argl = 0;
    
    //lecture des options
    while(1) {
        int opt;

        opt = getopt(argc, argv, "d:p:P:w:h:l:fx"); //"ab::c:" argless a, optarg b, mandatoryarg c
        if(opt < 0)
            break;
	
        switch(opt) {
	case 'p': // "p" comme "palette statique"
	    if(argP != 0) {
		fprintf(stderr, "You must choose between \"p\" and \"P\" options.");
		return 1;
	    }
	    if(optarg != NULL) 	argp = atoi(optarg);
	    else argp = 0;
	    break;
	case 'P': // "P" comme "palette dynamique"
	    if(argp != 0) {
		fprintf(stderr, "You must choose between \"p\" and \"P\" options.");
		return 1;
	    } else if(argP < 0 || argP > 256) {
		fprintf(stderr, "The palette can't exceed 256 colors (and obviously can't be negative).");
		return 1;
	    }
	    if(optarg != NULL) argP = atoi(optarg);
	    else argP = 0 ;
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
	case 'l' : // "l" comme "espace chromatique"
	    if(optarg != NULL) argl = atoi(optarg);
	    else argl = 0;
	    break;
	case 'f': // "f" comme "fast"
	    argf = 1;
	    break;
	case 'x': // "x" comme "execution"
	    argx = 1;
	    break;
        default:
            return 1;
        }
    }

    if(argc != optind + 1 && argc != optind + 2) {
        usage(stdout, argv);
        return 1;
    }
    
    //lecture de l'image source et mise en mémoire
    img = read_png(argv[optind]);
    if(img == NULL) {
        fprintf(stderr, "Couldn't read %s\n", argv[optind]);
        return 1;
    }

    //changement de l'espace colorimétrique
    if ( argl == 1 ) {
	image_rgb_to_lab ( img ) ;
    } else if ( argl == 2 ) {
	image_rgb_to_luv ( img ) ;
    } else {
	fprintf(stderr , "-l : bad argument\n") ;
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
	free_image(img);
        return 1;
    }

    //création de la palette de couleur
    switch(argp) {
    case 8:
	printf("Palette de 8 couleurs : saturation\n");
	palette_8(pali); //définition palette de 8 couleurs
	break;
    case 16:
	printf("Palette de 16 couleurs : CGA\n");
	palette_16(pali);
	break;
    case 64:
	printf("Palette de 64 couleurs : 4-4-4\n");
	palette_64(pali);
	break;
    case 216:
	printf("Palette de 216 couleurs : 6-6-6\n");
	palette_216(pali);
	break;
    case 252:
	printf("Palette de 252 couleurs : 6-7-6\n");
	palette_252(pali);
	break;
    case 2:
	printf("Palette de 2 couleurs : noir et blanc\n");
	palette_2(pali);
	break;
    case 256:
	printf("Palette de 256 couleurs : niveaux de gris\n");
	palette_256(pali);
	break;
    case 0:
	if (argP > 1 && argf == 0) {
	    printf("Palette dynamique de %d couleurs : median cut\n", argP);
	    if (palette_dynamique_median_cut(pali, img, argP) == -1) {
		free_image(img);
		free_pal_image(pali);
		return 1;
	    }
	} else if (argP > 1 && argf == 1) {
	    printf("Palette dynamique de %d couleurs : couleurs les plus présentes\n", argP);
	    palette_dynamique( pali , img , argP );
	} else if (argP == 0) {
	    printf("Palette de 256 couleurs maximum\n");
	    printf("(ne fonctionne que sur les images de moins de 256 couleurs)\n");
	} else {
	    fprintf(stderr, "Bad argument\n");
	    return 1;
	}
	break;
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
	printf("Conversion en image indexée\n");
	if(naive_pal_image(pali, img) == -1) {
	    fprintf(stderr, "Conversion error\n");
	    free_image(img);
	    free_pal_image(pali);
	    return 1;
	}
	break;
    case 1:
	printf("Conversion en image indexée + dispersion d'erreur de Floyd-Steinberg\n");
	if(floydSteinberg_pal_image(pali, img) == -1) {
	    fprintf(stderr, "Conversion error\n");
	    free_image(img);
	    free_pal_image(pali);
	    return 1;
	}
	break;
    case 2:
	printf("Conversion en image indexée + dispersion d'erreur d'Atkinson\n");
	if(atkinson_pal_image(pali, img) == -1) {
	    fprintf(stderr, "Conversion error\n");
	    free_image(img);
	    free_pal_image(pali);
	    return 1;
	}
	break;
    case 3:
	printf("Conversion en image indexée + tramage ordonné\n");
	if(ordered_pal_image_216(pali, img) == -1) {
	    fprintf(stderr, "Conversion error\n");
	    free_image(img);
	    free_pal_image(pali);
	    return 1;
	}
	break;
    default:
	fprintf(stderr, "Invalid argument of option d\n");
	free_image(img);
	free_pal_image(pali);
	return 1;
    }
    
    //écriture de l'image générée
    if(argv[optind + 1] == NULL) {
        rc = write_pal_png("./output.png", pali);
	if(rc < 0) {
	    fprintf(stderr, "Couldn't write ./output.png\n");
	    free_image(img);
	    free_pal_image(pali);
	    return 1;
	}
    } else {
	rc = write_pal_png(argv[optind + 1], pali);
	if(rc < 0) {
	    fprintf(stderr, "Couldn't write %s.\n", argv[optind + 1]);
	    free_image(img);
	    free_pal_image(pali);
	    return 1;
	}
    }
    
    //free
    free_image(img);
    free_pal_image(pali);

    //ouverture automatique de l'image générée
    if(argx == 1) { 
	if(argv[optind + 1] == NULL) {
	    char cmd[] = "eog output.png";
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

void usage(FILE *f, char **argv) {
    fprintf(f, "Usage: %s [OPTIONS] SOURCE DEST\n", argv[0]);
    fprintf(f, "Compress Portable Network Graphics (PNG) SOURCE to DEST\n");
    fprintf(f, "\nExamples:\n");
    fprintf(f, "  ./compresspng -p 252 image.png                      # Compress an image into an indexed image with a static palette of 252 colors named \"output.png\"\n");
    fprintf(f, "  ./compresspng -p 8 image.png image_saturated.png    # Compress an image into an indexed image with a static palette of 8 colors named \"image_saturated.png\"\n");
    fprintf(f, "\nOptions:\n");
    fprintf(f, "  -p [ARG]                   choose the size of the static palette : ARG = {2,8,16,64,216,252,256}\n");
    fprintf(f, "  -P [ARG]                   choose the size of the dynamic palette (default : median cut) : ARG = multiple of 2 <= 256\n");
    fprintf(f, "  -d [ARG]                   choose the algorithm of dithering/error diffusion : ARG = (1) Floyd-Steinberg, (2) Atkinson, (3) ordered\n");
    fprintf(f, "                             \n");
    fprintf(f, "  -h [ARG]                   choose the height of the new image (in pixels)\n");
    fprintf(f, "  -w [ARG]                   choose the width of the new image (in pixels)\n");
    fprintf(f, "  -f                         \"fast\" : to associated with the 'P' option to get the \"most common colors\" dynamic palette algorithm\n");
    fprintf(f, "  -x                         open the new image in Eye of Gnome (Ubuntu's default image viewer)\n\n");
    fprintf(f, "Louise Lemaire & Benoît Montazeaud\n");
}


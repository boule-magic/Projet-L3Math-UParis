#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#include "pngio.h"

void
free_image(struct image *image)
{
    for(int i = 0; i < image->height; i++)
        free(image->data[i]);
    free(image->data);
    free(image);
}

void
free_pal_image(struct pal_image *image)
{
    for(int i = 0; i < image->height; i++)
        free(image->data[i]);
    free(image->data);
    free(image->pal);
    free(image);
}


struct image *
read_png(char *filename) {
    FILE *f;
    unsigned char header[8];
    png_structp pngp = NULL;
    png_infop infop = NULL;
    int numrows = 0;
    int ctype, depth;
    unsigned char **rows = NULL;
    struct image *image = NULL;
    int rc;

    f = fopen(filename, "r");
    if(f == NULL) {
        perror("fopen");
        return NULL;
    }

    rc = fread(header, 1, 8, f);
    if(rc != 8) {
        perror("read(header)");
        goto fail;
    }

    if(png_sig_cmp(header, 0, 8) != 0) {
        fprintf(stderr, "Not a PNG file.\n");
        goto fail;
    }

    pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     NULL, NULL, NULL);
    if(pngp == NULL)
        goto fail;

    infop = png_create_info_struct(pngp);
    if(infop == NULL)
        goto fail;

    if(setjmp(png_jmpbuf(pngp))) {
        fprintf(stderr, "Error while reading PNG file.\n");
        goto fail;
    }

    png_init_io(pngp, f);
    png_set_sig_bytes(pngp, 8);

    png_read_info(pngp, infop);

    ctype = png_get_color_type(pngp, infop);
    if(ctype != (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)) {
        fprintf(stderr, "Bad color type %d.\n", ctype);
        goto fail;
    }
    depth = png_get_bit_depth(pngp, infop);
    if(depth != 8) {
        fprintf(stderr, "Color depth is %d, expected 8.\n", depth);
        goto fail;
    }

    numrows = png_get_image_height(pngp, infop);
    rows = calloc(numrows, sizeof(unsigned char *));
    if(rows == NULL)
        goto fail;
    for(int i = 0; i < numrows; i++) {
        rows[i] = malloc(png_get_rowbytes(pngp, infop));
        if(rows[i] == NULL)
            goto fail;
    }

    png_read_image(pngp, rows);

    image = malloc(sizeof(struct image));
    if(image == NULL)
        goto fail;
    image->data = rows;
    image->height = png_get_image_height(pngp, infop);
    image->width = png_get_image_width(pngp, infop);

    png_destroy_read_struct(&pngp, &infop, NULL);
    fclose(f);

    return image;

 fail:
    png_destroy_read_struct(&pngp, &infop, NULL);

    if(image != NULL) {
        free(image->data);
        free(image);
    }

    if(rows != NULL) {
        for(int i = 0; i < numrows; i++)
            free(rows[i]);
        free(rows);
    }

    fclose(f);
    return NULL;
}

int
write_png(char *file, const struct image *image)
{
    FILE *f;
    png_structp pngp = NULL;
    png_infop infop = NULL;

    f = fopen(file, "w");
    if(f == NULL) {
        perror("fopen");
        return -1;
    }

    pngp = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                   NULL, NULL, NULL);
    if(pngp == NULL)
        goto fail;

    infop = png_create_info_struct(pngp);
    if(infop == NULL)
        goto fail;

    if(setjmp(png_jmpbuf(pngp))) {
        fprintf(stderr, "Unable to write PNG file.\n");
        goto fail;
    }

    png_init_io(pngp, f);

    png_set_IHDR(pngp, infop,
                 image->width, image->height,
                 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_set_rows(pngp, infop, image->data);

    png_write_png(pngp, infop, PNG_TRANSFORM_IDENTITY, NULL);

    png_destroy_write_struct(&pngp, &infop);
    fclose(f);
    return 1;

 fail:
    png_destroy_write_struct(&pngp, &infop);
    fclose(f);
    return -1;
}

int
write_pal_png(char *file, const struct pal_image *image)
{
    FILE *f;
    png_structp pngp = NULL;
    png_infop infop = NULL;
    png_colorp pal = NULL;

    f = fopen(file, "w");
    if(f == NULL) {
        perror("fopen");
        return -1;
    }

    pngp = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                   NULL, NULL, NULL);
    if(pngp == NULL)
        goto fail;

    infop = png_create_info_struct(pngp);
    if(infop == NULL)
        goto fail;

    if(setjmp(png_jmpbuf(pngp))) {
        fprintf(stderr, "Unable to write PNG file.\n");
        goto fail;
    }

    png_init_io(pngp, f);

    png_set_IHDR(pngp, infop,
                 image->width, image->height,
                 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_set_rows(pngp, infop, image->data);

    pal = malloc(image->pal_len * sizeof(png_color));
    if(pal == NULL)
        goto fail;

    for(int i = 0; i < image->pal_len; i++) {
        pal[i].red = image->pal[3 * i];
        pal[i].green = image->pal[3 * i + 1];
        pal[i].blue = image->pal[3 * i + 2];
    }
    png_set_PLTE(pngp, infop, pal, image->pal_len);

    png_write_png(pngp, infop, PNG_TRANSFORM_IDENTITY, NULL);

    free(pal);
    pal = NULL;
    png_destroy_write_struct(&pngp, &infop);
    fclose(f);
    return 1;

 fail:
    free(pal);
    png_destroy_write_struct(&pngp, &infop);
    fclose(f);
    return -1;
}

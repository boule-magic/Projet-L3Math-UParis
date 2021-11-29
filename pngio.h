#ifndef PNGIO_H
#define PNGIO_H

struct image {
    unsigned char **data;
    int height, width;
};

struct pal_image {
    unsigned char **data;
    int height, width;
    unsigned char *pal;
    int pal_len;
};

void free_image(struct image *image);
void free_pal_image(struct pal_image *image);
struct image *read_png(char *filename);
int write_png(char *file, const struct image *image);
int write_pal_png(char *file, const struct pal_image *image);

#endif

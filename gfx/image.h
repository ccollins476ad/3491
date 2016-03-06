#ifndef IMAGE_H_
#define IMAGE_H_

struct BITMAP;

struct image_t {
    struct BITMAP *bmp;
    char *filename;
    double xscale;
    double yscale;
};

struct BITMAP *image_bmp(struct image_t *image);

#endif


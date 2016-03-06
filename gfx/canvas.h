#ifndef H_CANVAS_
#define H_CANVAS_

struct BITMAP;

struct canvas_t {
    int x;
    int y;
    int dimx;
    int dimy;
    int color;
    struct BITMAP *bmp;
};

void canvas_set_clip(struct canvas_t *canvas);
void canvas_clear(struct canvas_t *canvas);
void canvas_coord(struct canvas_t *canvas, int x, int y, int *xout, int *yout);

#endif


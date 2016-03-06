#include "gen/gen_num.h"
#include "gfx/balleg.h"
#include "gfx/canvas.h"
#include "gfx/periph.h"

void
periph_circ_draw(struct canvas_t *canvas, int canvasx, int canvasy, int radius,
                 int color, int angle)
{
    int bmpx;
    int bmpy;

    bmpx = canvasx + canvas->x;
    bmpy = canvasy + canvas->y;

    circle(canvas->bmp, bmpx, bmpy, radius, color);
}

void
periph_dot_draw(struct canvas_t *canvas, int canvasx, int canvasy, int radius,
                int color, int angle)
{
    int bmpx;
    int bmpy;
    int px;
    int py;

    bmpx = canvasx + canvas->x;
    bmpy = canvasy + canvas->y;

    angle_to_vect(angle, radius, &px, &py);
    putpixel(canvas->bmp, bmpx + px, bmpy + py, color);
}

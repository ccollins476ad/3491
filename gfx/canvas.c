#include "gfx/gfx.h"
#include "gfx/balleg.h"
#include "gfx/canvas.h"

void
canvas_set_clip(struct canvas_t *canvas)
{
    set_clip_rect(canvas->bmp, canvas->x, canvas->y,
                  canvas->x + canvas->dimx, canvas->y + canvas->dimy);
}

void
canvas_clear(struct canvas_t *canvas)
{
    rectfill(canvas->bmp, canvas->x, canvas->y,
             canvas->x + canvas->dimx, canvas->y + canvas->dimy,
             canvas->color);
}

void
canvas_coord(struct canvas_t *canvas, int x, int y, int *xout, int *yout)
{
    *xout = canvas->x + x;
    *yout = canvas->y + y;
}

#include "gfx/tile_shape.h"

void
tile_shape_rect(struct canvas_t *canvas, int x, int y, int dimx, int dimy,
                tile_id_t tile_id, uint32_t fg, uint32_t bg)
{
    int curx;
    int cury;
    int i;

    if (dimx <= 0 || dimy <= 0) {
        return;
    }

    for (i = 0; i < dimx; i++) {
        curx = (x + i) * TILE_DIM;

        cury = y * TILE_DIM;
        tile_draw_bg(canvas, curx, cury, tile_id, fg, bg);

        cury = (y + dimy - 1) * TILE_DIM;
        tile_draw_bg(canvas, curx, cury, tile_id, fg, bg);
    }

    for (i = 1; i < dimy - 1; i++) {
        cury = (y + i) * TILE_DIM;

        curx = x * TILE_DIM;
        tile_draw_bg(canvas, curx, cury, tile_id, fg, bg);

        curx = (x + dimx - 1) * TILE_DIM;
        tile_draw_bg(canvas, curx, cury, tile_id, fg, bg);
    }
}

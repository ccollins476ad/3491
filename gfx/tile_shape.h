#ifndef H_TILE_SHAPE_
#define H_TILE_SHAPE_

#include "tile.h"

void
tile_shape_rect(struct canvas_t *canvas, int x, int y, int dimx, int dimy,
                tile_id_t tile_id, uint32_t fg, uint32_t bg);

#endif

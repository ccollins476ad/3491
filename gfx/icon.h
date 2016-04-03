#ifndef H_ICON_
#define H_ICON_

#include <inttypes.h>
#include "gfx/tile.h"

struct icon_t {
    tile_id_t tile_id;
    uint32_t fg;
    uint32_t bg;
};

#endif


#include "gfx/balleg.h"
#include "gfx/gfx.h"
#include "life/terr.h"
#include "life/terr_defs.h"
#include "life/data.h"

struct terr_properties_t data_terr[TERR_ID_MAX] = {
    [TERR_ID_AIR_BLACK] = {
        .icon = { .tile_id = 219, .fg = COLOR(0, 0, 0) },
        .flags = 0,
    },
    [TERR_ID_SAND] = {
        .icon = { .tile_id = 219, .fg = COLOR(100, 75, 0) },
        .flags = 0,
    },
    [TERR_ID_STEEL_WALL] = {
        .icon = { .tile_id = 219, .fg = WHITE(150) },
        .flags = TERR_F_SOLID | TERR_F_OPAQUE,
    },
    [TERR_ID_DARK_GRAY_WALL] = {
        .icon = { .tile_id = 219, .fg = COLOR(100, 100, 100) },
        .flags = TERR_F_SOLID | TERR_F_OPAQUE,
    },
    [TERR_ID_DIRT] = {
        .icon = { .tile_id = 219, .fg = COLOR(60, 30, 0) },
        .flags = 0,
    },
    [TERR_ID_LIT_WINDOW] = {
        .icon = { TILE_ID_BLOCK_100, COLOR(255, 255, 100), WHITE(150) },
        .flags = 0,
    },
    [TERR_ID_OFF_WINDOW] = {
        .icon = { TILE_ID_BLOCK_100, COLOR(50, 50, 50), WHITE(150) },
        .flags = 0,
    },
    [TERR_ID_GRAY_RUBBLE] = {
        .icon = { .tile_id = TILE_ID_FACE_SOLID, .fg = COLOR(240, 240, 240) },
        .flags = 0,
    },
};

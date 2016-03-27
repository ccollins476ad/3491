#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "life/terr.h"
#include "life/terr_defs.h"
#include "life/world.h"
#include "life/data.h"
#include "life/level.h"

struct level_t *level_cur;

static int
level_num_rows(struct level_t *level)
{
    int i;

    i = 0;
    while (level->rows[i] != NULL) {
        i++;
    }

    return i;
}

static struct level_tile_t *
level_lookup_tile(struct level_t *level, char c)
{
    struct level_tile_t *tile;

    for (tile = level->tiles; tile != NULL; tile++) {
        if (tile->key == c) {
            return tile;
        }
    }

    return NULL;
}

int
level_load(struct level_t *level)
{
    struct level_tile_t *tile;
    char c;
    int dimx;
    int dimy;
    int x;
    int y;
    int i;

    dimy = level_num_rows(level);
    dimx = strlen(level->rows[0]);
    for (i = 1; i < dimy; ++i) {
        if (strlen(level->rows[i]) != dimx) {
            assert(0);
            return -1;
        }
    }

    terr_scape_create(dimx, dimy, TERR_ID_AIR_BLACK);
    for (y = 0; y < dimy; ++y) {
        for (x = 0; x < dimx; ++x) {
            c = level->rows[y][x];

            tile = level_lookup_tile(level, c);
            assert(tile != NULL);
            terr_scape_set(x, y, tile->terr_id);
        }
    }

    level_cur = level;

    return 0;
}

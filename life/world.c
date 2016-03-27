#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include "gen/gen_dbg.h"
#include "life/zone.h"
//#include "life/hit_box.h"
#include "life/terr.h"
#include "life/world.h"

unsigned int world_tick;

int
world_scape_width(void)
{
    return terr_scape.width * WORLD_TILE_DIM;
}

int
world_scape_height(void)
{
    return terr_scape.height * WORLD_TILE_DIM;
}

void
world_fill_zone(void)
{
    zone_beingx = world_view_info.worldx - WORLD_ZONE_BEING_MARGIN;
    zone_beingy = world_view_info.worldy - WORLD_ZONE_BEING_MARGIN;
    zone_being_dimx = WORLD_ZONE_BEING_DIMX;
    zone_being_dimy = WORLD_ZONE_BEING_DIMY;

    zone_bulletx = world_view_info.worldx - WORLD_ZONE_BULLET_MARGIN;
    zone_bullety = world_view_info.worldy - WORLD_ZONE_BULLET_MARGIN;
    zone_bullet_dimx = WORLD_ZONE_BULLET_DIMX;
    zone_bullet_dimy = WORLD_ZONE_BULLET_DIMY;

#if 0
    hit_box_set_bounds(zone_bulletx, zone_bullety,
                       zone_bullet_dimx, zone_bullet_dimy);
#endif
}

int
world_segment_tile_foreach(int x1, int y1, int x2, int y2,
                           segment_tile_foreach_cb_t cb, void *arg)
{
    return segment_tile_foreach(x1, y1, x2, y2,
                                WORLD_TILE_DIM, WORLD_TILE_DIM, cb, arg);
}


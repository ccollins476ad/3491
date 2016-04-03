#include <assert.h>
#include "life/player.h"
#include "life/being.h"
#include "life/world.h"
#include "life/terr.h"
#include "life/scroll.h"

struct scroll_data_t scroll_data;

static void
scroll_set_center(int centerx, int centery)
{
    int recenterx;
    int recentery;
    int overlapr;
    int overlapb;
    int scapew;
    int scapeh;
    int viewr;
    int viewb;

    scapew = world_scape_width();
    scapeh = world_scape_height();

    if (scapew < world_view_info.world_dimx) {
        centerx = scapew / 2;
        recenterx = 1;
    } else {
        recentery = 0;
    }

    if (scapeh < world_view_info.world_dimy) {
        centery = scapeh / 2;
        recentery = 1;
    } else {
        recentery = 0;
    }

    if (!recenterx || !recentery) {
        world_view_set_center(centerx, centery);
    }

    if (!recenterx) {
        if (world_view_info.worldx < 0) {
            centerx -= world_view_info.worldx;
            recenterx = 1;
        } else {
            viewr = world_view_info.worldx + world_view_info.world_dimx;
            overlapr = viewr - scapew + 1;
            if (overlapr > 0) {
                centerx -= overlapr;
                recenterx = 1;
            }
        }
    }

    if (!recentery) {
        if (world_view_info.worldy < 0) {
            centery -= world_view_info.worldy;
            recentery = 1;
        } else {
            viewb = world_view_info.worldy + world_view_info.world_dimy;
            overlapb = viewb - scapeh + 1;
            if (overlapb > 0) {
                centery -= overlapb;
                recentery = 1;
            }
        }
    }

    if (recenterx || recentery) {
        world_view_set_center(centerx, centery);
    }
}

static void
scroll_update_mode_player(void)
{
    scroll_set_center(player.being->phys.x, player.being->phys.y);
}

static void
scroll_update_mode_free(void)
{
    scroll_set_center(scroll_data.centerx, scroll_data.centery);
}

void
scroll_update(void)
{
    switch (scroll_data.mode) {
    case SCROLL_MODE_PLAYER:
        scroll_update_mode_player();
        break;

    case SCROLL_MODE_FREE:
        scroll_update_mode_free();
        break;

    default:
        assert(0);
        break;
    }
}


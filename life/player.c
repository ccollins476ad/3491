#include <stddef.h>
#include "gfx/periph.h"
#include "gfx/canvas.h"
#include "life/being_defs.h"
#include "life/being.h"
#include "life/world.h"
#include "life/player.h"

struct player_t player;

void
player_update_visible(void)
{
    int centerx;
    int centery;

    being_center(player.being, &centerx, &centery);
    being_batch_set_visible(centerx, centery);
}



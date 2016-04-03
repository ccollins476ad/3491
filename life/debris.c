#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gen/gen_num.h"
#include "life/phys.h"
#include "life/world.h"
#include "life/debris.h"

void
erupt_apply(struct erupt_t *erupt, struct phys_t *phys)
{
    double angle;
    int speed;

    speed = erupt->speed_base + rand_up_to(erupt->speed_rand);
    angle = erupt->angle_base + rand_up_to(erupt->angle_rand);

    phys->xspeed = speed * cos(angle);
    phys->yspeed = speed * -sin(angle);
}

void
particle_draw(struct particle_t *particle)
{
    struct icon_t icon;
    int minc;
    int maxc;

    minc = world_view_info.worldx - WORLD_TILE_DIM;
    maxc = world_view_info.worldx + world_view_info.world_dimx;
    if (particle->phys.x < minc || particle->phys.x >= maxc) {
        return;
    }

    minc = world_view_info.worldy - WORLD_TILE_DIM;
    maxc = world_view_info.worldy + world_view_info.world_dimy;
    if (particle->phys.y < minc || particle->phys.y >= maxc) {
        return;
    }

    animation_player_icon(&particle->animation_player, &icon);
    world_view_draw_icon(&icon, NULL, particle->phys.x, particle->phys.y);
}

int
particle_update(struct particle_t *particle)
{
    animation_player_update(&particle->animation_player);
    if (!animation_player_active(&particle->animation_player)) {
        return 1;
    } else {
        phys_update(&particle->phys);
        return 0;
    }
}



#include "life/phys.h"

void
phys_update(struct phys_t *phys)
{
    phys->xspeed += phys->xaccel;
    phys->yspeed += phys->yaccel;
    phys->x += phys->xspeed;
    phys->y += phys->yspeed;
}

void
phys_apply_friction(struct phys_t *phys, double friction)
{
    phys->xspeed *= friction;
    phys->yspeed *= friction;
}

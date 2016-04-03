#ifndef PHYS_H_
#define PHYS_H_

struct phys_t {
    int x;
    int y;
    int xspeed;
    int yspeed;
    int xaccel;
    int yaccel;
};

void phys_update(struct phys_t *phys);
void phys_apply_friction(struct phys_t *phys, double friction);

#endif

#ifndef DEBRIS_H_
#define DEBRIS_H_

#include "life/phys.h"
#include "gfx/animation.h"

#define PARTICLE_F_FIRST_TICK           0x01

struct phys_t;

struct erupt_t {
    int speed_base;
    int speed_rand;
    int angle_base;
    int angle_rand;
};

struct particle_properties_t {
    struct animation_t *animation;
    uint8_t flags;
};

struct particle_t {
    struct particle_properties_t *properties;
    struct phys_t phys;
    struct animation_player_t animation_player;
    uint8_t flags;
};

void erupt_apply(struct erupt_t *erupt, struct phys_t *phys);

void particle_draw(struct particle_t *particle);
int particle_update(struct particle_t *particle);

#endif


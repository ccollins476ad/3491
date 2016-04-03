#include <math.h>
#include "gfx/gfx.h"
#include "life/being.h"
#include "life/being_defs.h"
#include "life/data.h"
#include "life/hit_box.h"
#include "life/world.h"

struct being_properties_t data_being_properties[BEING_TYPE_MAX] = {
    [BEING_TYPE_RALF] = {
        .type = BEING_TYPE_RALF,
        .fsm = &data_shp1_being_fsm,
        .icon = { TILE_ID_FACE_SOLID, WHITE(255) },
        .walk_speed     = WORLD_FACTOR * 0.05,
        .run_speed      = WORLD_FACTOR * 0.30,
        .max_speed      = WORLD_FACTOR * 6,
        .turn_rate_slow = 0.0050 * M_PI,
        .turn_rate_fast = 0.0100 * M_PI,
        .defense = {
            .hit_points = INT_MAX,
            .damaged_by = ATTACK_MASK_ALL_TYPES,
            .flags = 0,
        },
        .flags = 0,//BEING_F_PERIPH | BEING_F_DOT,
    },

    [BEING_TYPE_CIV1] = {
        .type = BEING_TYPE_CIV1,
        .fsm = &data_shp1_being_fsm,
        .icon = { TILE_ID_FACE_SOLID, COLOR(150, 200, 250) },
        .walk_speed     = WORLD_FACTOR * 0.25,
        .run_speed      = WORLD_FACTOR * 0.75,
        .turn_rate_slow = 0.005 * M_PI,
        .turn_rate_fast = 0.015 * M_PI,
        .defense = {
            .hit_points = 1000,
            .damaged_by = ATTACK_MASK_ALL_TYPES,
            .flags = 0,
        },
        .flags = BEING_F_DOT,
    },
};

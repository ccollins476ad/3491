#include "gfx/gfx.h"
#include "gfx/animation.h"
#include "life/blood.h"
#include "life/data.h"

struct blood_properties_t data_blood_properties[BLOOD_TYPE_MAX] = {
    [BLOOD_TYPE_DROP] = {
        .particle_properties = {
            .animation = (struct animation_t[1]) { {
                .frames = (struct animation_frame_t[]) {
                    [0] = { .icon = { 7,   COLORA(200, 0, 0, 255), } },
                    [1] = { .icon = { 7,   COLORA(200, 0, 0, 220), } },
                    [2] = { .icon = { 7,   COLORA(200, 0, 0, 180), } },
                    [3] = { .icon = { 250, COLORA(200, 0, 0, 150), } },
                    [4] = { .icon = { 250, COLORA(200, 0, 0, 120), } },
                    [5] = { .icon = { 250, COLORA(200, 0, 0, 90), } },
                },
                .num_frames = 6,
            } },
        },
    },
};

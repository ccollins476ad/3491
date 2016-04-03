#include "gfx/gfx.h"
#include "gfx/animation.h"
#include "life/spark.h"
#include "life/data.h"

struct spark_properties_t data_spark_properties[SPARK_TYPE_MAX] = {
    [SPARK_TYPE_EXPLOSION] = {
        .particle_properties = {
            .animation = (struct animation_t[1]) { {
                .frames = (struct animation_frame_t[]) {
                    [0] = { .icon = { TILE_ID_BLOCK_100, WHITE(255) } },
                    [1] = { .icon = { TILE_ID_BLOCK_100, WHITE(150) } },
                    [2] = { .icon = { TILE_ID_BLOCK_100, WHITE(100) } },
                    [3] = { .icon = { TILE_ID_BLOCK_100, WHITE(50) } },
                },
                .num_frames = 4,
            } },
        },
    },
    [SPARK_TYPE_FIRE] = {
        .particle_properties = {
            .animation = (struct animation_t[1]) { {
                .frames = (struct animation_frame_t[]) {
                    [0] = { .icon = { TILE_ID_BLOCK_100, COLOR(255, 255, 0) } },
                    [1] = { .icon = { TILE_ID_BLOCK_100, COLOR(255, 255, 0) } },
                    [2] = { .icon = { TILE_ID_BLOCK_100, COLOR(255, 200, 0) } },
                    [3] = { .icon = { TILE_ID_BLOCK_100, COLOR(255, 150, 0) } },
                    [4] = { .icon = { 7, COLOR(200, 100, 0) } },
                    [5] = { .icon = { 7, COLOR(150, 75, 75) } },
                    [6] = { .icon = { 7, COLOR(100, 75, 75) } },
                    [7] = { .icon = { 250, COLOR(75, 75, 75) } },
                    [8] = { .icon = { 250, COLOR(40, 40, 40) } },
                },
                .num_frames = 9,
            } },
        },
    },
    [SPARK_TYPE_SMOKE] = {
        .particle_properties = {
            .animation = (struct animation_t[1]) { {
                .frames = (struct animation_frame_t[]) {
                    [0] = { .icon = { 7, WHITEA(200, 200) } },
                    [1] = { .icon = { 7, WHITEA(200, 180) } },
                    [2] = { .icon = { 7, WHITEA(200, 160) } },
                    [3] = { .icon = { 7, WHITEA(200, 140) } },
                    [4] = { .icon = { 7, WHITEA(200, 120) } },
                    [5] = { .icon = { 7, WHITEA(200, 100) } },
                    [6] = { .icon = { 7, WHITEA(200, 80) } },
                    [7] = { .icon = { 7, WHITEA(200, 60) } },
                    [8] = { .icon = { 7, WHITEA(200, 40) } },
                    [9] = { .icon = { 7, WHITEA(200, 20) } },
                },
                .num_frames = 10,
            } },
        },
    },
};

#ifndef H_ANIMATION_
#define H_ANIMATION_

#include "gfx/icon.h"

#define ANIMATION_STATIC_DURATION_NONE  0

struct animation_frame_t {
    struct icon_t icon;
    int duration;
};

struct animation_t {
    struct animation_frame_t *frames;
    int num_frames;
};

struct animation_player_t {
    struct animation_t *animation;
    tile_id_t static_tile;
    int static_duration;

    int frame_idx;
    int frame_tick;
};

int animation_player_active(struct animation_player_t *player);
void animation_player_start(struct animation_player_t *player,
                            struct animation_t *animation,
                            tile_id_t static_tile, int static_duration);
void animation_player_update(struct animation_player_t *player);
void animation_player_icon(struct animation_player_t *player,
                           struct icon_t *icon);

#endif


#include <stddef.h>
#include <assert.h>
#include "gfx/animation.h"

int
animation_player_active(struct animation_player_t *player)
{
    return player->animation != NULL &&
           player->frame_idx < player->animation->num_frames;
}

void
animation_player_start(struct animation_player_t *player,
                       struct animation_t *animation,
                       tile_id_t static_tile, int static_duration)
{
    player->animation = animation;
    player->static_tile = static_tile;
    player->static_duration = static_duration;

    player->frame_idx = 0;
    player->frame_tick = 0;
}

static int
animation_player_cur_duration(struct animation_player_t *player)
{
    if (player->static_duration != ANIMATION_STATIC_DURATION_NONE) {
        return player->static_duration;
    }

    return player->animation->frames[player->frame_idx].duration;
}

void
animation_player_update(struct animation_player_t *player)
{
    assert(animation_player_active(player));

    player->frame_tick++;
    if (player->frame_tick >= animation_player_cur_duration(player)) {
        player->frame_tick = 0;
        player->frame_idx++;
    }
}

void
animation_player_icon(struct animation_player_t *player, struct icon_t *icon)
{
    struct animation_frame_t *frame;

    assert(animation_player_active(player));

    frame = player->animation->frames + player->frame_idx;
    *icon = frame->icon;
    if (player->static_tile != TILE_ID_NONE) {
        icon->tile_id = player->static_tile;
    }
}


#include <stdlib.h>
#include <string.h>
#include "life/hit_box.h"
#include "life/action.h"

/*****************************************************************************
 * @action_player                                                            *
 *****************************************************************************/

/**
 * Returns 1 if a frame transition occurs; 0 otherwise.
 */
int
action_player_update(struct action_player_t *player)
{
    int duration;

    duration = action_player_frame_duration(player);
    if (duration == ACTION_FRAME_DURATION_INF) {
        return 0;
    }

    ++player->tick;
    if (player->tick < duration) {
        return 0;
    }

    action_player_set_frame(player, player->frame->next_frame,
                            ACTION_FRAME_EXPLICIT_DURATION_NONE);
    return 1;
}

void
action_player_set_frame(struct action_player_t *player,
                        struct action_frame_t *frame,
                        int explicit_duration)
{
    player->frame = frame;
    player->tick = 0;
    player->explicit_dur = explicit_duration;
}

int
action_player_frame_duration(struct action_player_t *player)
{
    if (player->explicit_dur == ACTION_FRAME_EXPLICIT_DURATION_NONE) {
        return player->frame->duration;
    } else {
        return player->explicit_dur;
    }
}


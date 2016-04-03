#ifndef ACTION_H_
#define ACTION_H_

#include <stdint.h>
#include "life/action_defs.h"
struct image_t;
struct being_t;
struct hit_box_desc_t;

#define ACTION_FRAME_MAX_EFFECTS    16

struct action_frame_t {
    struct icon_t *icon;
    struct action_frame_t *next_frame;

    /**
     * (prop-id, value)-pairs; terminated with (PROP_ID_NULL, 0);
     * NULL for no kv.
     */
    int (*kv)[2];

    struct hit_box_desc_t *hit_boxes;
    int *effects;
    int duration;
};

struct action_player_t {
    struct action_frame_t *frame;
    int tick;
    int explicit_dur;
};

int action_player_update(struct action_player_t *player);
void action_player_set_frame(struct action_player_t *player,
                             struct action_frame_t *frame,
                             int explicit_duration);
int action_player_frame_duration(struct action_player_t *player);

#endif


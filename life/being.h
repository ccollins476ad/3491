#ifndef H_BEING_
#define H_BEING_

#include "gfx/fader.h"
#include "life/action.h"
#include "life/combat.h"
#include "life/entity.h"
#include "life/fsm.h"
#include "life/gun.h"
#include "life/phys.h"
struct being_interest_entry_t;
struct being_traits_entry_t;
struct image_t;
struct level_t;

typedef uint16_t being_flags_t;

struct being_interest_entry_t {
    struct being_interest_entry_t *next;

    struct entity_t *entity;
    int purpose;
};

struct being_traits_entry_t {
    struct being_traits_entry_t *next;

    int type;
    int val;
};

struct being_fsm_t {
    struct action_frame_t **state_frame_map; /* [state_id] = frame. */
    struct fsm_graph_t graph;

    struct action_frame_t *frames;
    int num_frames;
    struct const_name_pair_t *frame_names;
};

struct being_properties_t {
    int type;

    struct being_fsm_t *fsm;
    struct icon_t icon;
    struct defense_properties_t defense;

    // XXX Probably replace with per-being traits.
    int walk_speed;
    int run_speed;
    double turn_rate_slow;
    double turn_rate_fast;
    being_flags_t flags;
};

struct being_log_ctxt {
    unsigned fsm                :1;
    unsigned ai                 :1;
};

struct being_t {
    struct entity_t entity;

    being_flags_t flags;
    struct phys_t phys;
    double angle; /* Rads. */
    struct action_player_t action_player;
    struct being_properties_t *properties;
    struct gun_t gun;
    int hit_points;
    int team_id;

    struct image_t *image;
    struct fader_t fader;
    struct fsm_ctxt_t fsm_ctxt;

    struct being_log_ctxt log_ctxt;

    struct being_interest_entry_t *interest_list;
    struct being_traits_entry_t *traits_list;
};

struct being_data_t {
    struct slab_t being_slab;
    struct slab_t interest_slab;
    struct slab_t traits_slab;
};
extern struct being_data_t being_data;

/*****************************************************************************
 * @being                                                                    *
 *****************************************************************************/

void being_signal(struct being_t *being, struct entity_t *sender,
                  uint8_t signal);

struct being_interest_entry_t *
being_add_interest_and_signal(struct being_t *being,
                              struct entity_t *sender,
                              int purpose, uint8_t signal);
void being_bleed_dir(struct being_t *being, int direction);

void being_icon(struct being_t *being, struct icon_t *icon);
void being_draw(struct being_t *being);
void being_update(struct being_t *being);

/*****************************************************************************
 * @being mgmt                                                               *
 *****************************************************************************/

struct being_interest_entry_t *
being_interest_find(struct being_t *being,
                    struct entity_t *entity,
                    int purpose,
                    struct being_interest_entry_t *sought,
                    struct being_interest_entry_t **prev_out);
void
being_interest_remove(struct being_t *being,
                      struct being_interest_entry_t *entry,
                      struct being_interest_entry_t *prev);

struct being_interest_entry_t *being_interest_add(struct being_t *being);

struct being_traits_entry_t *
being_traits_find(struct being_t *being, int type,
                  struct being_traits_entry_t **prev_out);
int being_traits_find_dflt(struct being_t *being, int type, int dflt);
struct being_traits_entry_t *being_traits_add(struct being_t *being);
struct being_traits_entry_t *being_traits_ensure(struct being_t *being,
                                                 int type);
int being_traits_replace(struct being_t *being, int type, int val);
void being_traits_remove(struct being_t *being,
                         struct being_traits_entry_t *entry,
                         struct being_traits_entry_t *prev);
void being_traits_remove_type(struct being_t *being, int type);
int being_traits_remove_dflt(struct being_t *being, int type, int dflt);

void being_center(struct being_t *being, int *outx, int *outy);
void being_fade(struct being_t *being);
void being_set_fsm_state(struct being_t *being, int state_id);
void being_teleport(struct being_t *being, struct level_t *level,
                    int x, int y);
int being_max_idx(void);
struct being_t *being_at(int idx);
void being_erase(struct being_t *being);
struct being_t *being_add(int type);
void being_clear(void);
void being_init(void);

/*****************************************************************************
 * @being batch                                                              *
 *****************************************************************************/

struct being_t *being_batch_find(unsigned int id, being_flags_t flags_any);
void being_batch_update(void);
void being_batch_process_collisions(void);
void being_batch_draw(void);
void being_batch_set_visible(int originx, int originy);

/*****************************************************************************
 * @being action                                                             *
 *****************************************************************************/

void being_update_action(struct being_t *being);
void being_signal(struct being_t *being, struct entity_t *sender,
                  uint8_t signal);
void being_set_action(struct being_t *being, struct action_frame_t *frame);

#endif

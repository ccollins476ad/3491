#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "gen/container.h"
#include "gen/direction.h"
#include "gen/gen_dbg.h"
#include "gen/gen_num.h"
#include "gfx/canvas.h"
#include "gfx/fader.h"
#include "gfx/gfx.h"
#include "gfx/periph.h"
#include "life/action.h"
#include "life/being.h"
#include "life/being_dbg.h"
#include "life/being_defs.h"
#include "life/blood.h"
#include "life/collision.h"
#include "life/data.h"
#include "life/event.h"
#include "life/generated/const_names.h"
#include "life/gun.h"
#include "life/hit_box.h"
#include "life/life_defs.h"
#include "life/phys.h"
#include "life/player.h"
#include "life/terr.h"
#include "life/world.h"
#include "life/zone.h"

#define BEING_BLEED_FRAME_DUR_BASE  (BLOOD_STAIN_SECOND / 25)
#define BEING_BLEED_FRAME_DUR_RAND  (BLOOD_STAIN_SECOND / 50)

#define BEING_PERIPH_PLAYER_C1      (COLOR(0, 100, 0))
#define BEING_PERIPH_PLAYER_C2      (COLOR(0, 200, 0))

#define BEING_PERIPH_ENEMY_C1       (COLOR(70, 70, 70))
#define BEING_PERIPH_ENEMY_C2       (COLOR(140, 140, 140))

/*****************************************************************************
 * @being interest                                                           *
 *****************************************************************************/

struct being_interest_entry_t *
being_add_interest_and_signal(struct being_t *being,
                              struct entity_t *sender,
                              int purpose, uint8_t signal)
{
    struct being_interest_entry_t *interest;
    struct being_interest_entry_t *prev;
    int rc;

    interest = being_interest_add(being);
    if (interest == NULL) {
        goto err;
    }

    rc = entity_add_signal(sender, &being->entity, signal);
    if (rc != 0) {
        goto err;
    }

    interest->entity = sender;
    interest->purpose = purpose;

    return interest;

err:
    if (interest != NULL) {
        /* Remove the bogus interest entry.  Need to find its previous entry
         * first.
         */
        interest = being_interest_find(being, NULL, BEING_PURPOSE_NONE,
                                       interest, &prev);
        if (interest != NULL) {
            being_interest_remove(being, interest, prev);
        }
    }

    return NULL;
}

/*****************************************************************************
 * @being misc                                                               *
 *****************************************************************************/

void
being_center(struct being_t *being, int *outx, int *outy)
{
    *outx = being->phys.x + WORLD_TILE_DIM / 2;
    *outy = being->phys.y + WORLD_TILE_DIM / 2;
}

/*****************************************************************************
 * @being hit box                                                            *
 *****************************************************************************/

static void
being_hit_box_build(struct being_t *being, struct entity_t *owner,
                    struct hit_box_desc_t *desc, struct hit_box_t *hit_box)
{
    hit_box_from_desc(hit_box, desc, being->phys.x, being->phys.y);
    hit_box->owner = owner;
}

/**
 * Returns 1 if a collision occurred.
 */
static int
being_hit_box_terr(struct being_t *being)
{
    struct hit_box_desc_t *cur_desc;
    struct action_frame_t *frame;
    struct hit_box_t hit_box;
    int cur_diffx;
    int cur_diffy;
    int diffx;
    int diffy;
    int sig;
    int dir;
    int rc;

    /* These variables track to total amount of movement applied to the being
     * as a result of terrain collisions.
     */
    diffx = 0;
    diffy = 0;

    frame = being->action_player.frame;
    HIT_BOX_DESCS_FOREACH(frame->hit_boxes, cur_desc) {
        if (cur_desc->flags & HIT_BOX_F_CLP) {
            being_hit_box_build(being, &being->entity, cur_desc, &hit_box);
            rc = collision_terr_hit_box_once(&hit_box, &cur_diffx, &cur_diffy);
            if (rc != COLLISION_TYPE_NONE) {
                being->phys.x += cur_diffx;
                being->phys.y += cur_diffy;

                diffx += cur_diffx;
                diffy += cur_diffy;

                /* Restart the loop in case this collision pushed an
                 * already-processed hit box into another piece of solid
                 * terrain.
                 */
                cur_desc = frame->hit_boxes;
            }
        }
    }

    if (diffx == 0 && diffy == 0) {
        return 0;
    }

    /* Process impact with terrain; send the appropriate signals to the being.
     * don't modify the being's location, speed, or acceleration yet.
     */
    dir = collision_terr_impact(being->phys.xspeed, being->phys.yspeed,
                                diffx, diffy);
    if (dir != DIRECTION_NONE) {
        sig = fsm_direction_bump_table[dir];
        fsm_ctxt_push_signal(&being->fsm_ctxt, sig);
    }

    return 1;
}

static void
being_fsm_cb(struct fsm_state_t *fsm_state, struct fsm_state_elem_t *elem,
             void *arg)
{
    struct action_frame_t *frame;
    struct being_t *being;

    being = arg;

    BEING_DBG_LOG_FSM(
        being,
        "%6d: id=%d %s (%d) + %s (%d) --> %s (%d) [%s (0x%x)]\n",
        world_tick, being->entity.id,
        const_names_get(being->properties->fsm->graph.names,
                        being->fsm_ctxt.state),
        being->fsm_ctxt.state,
        const_names_get(fsm_sig_names, elem->signal),
        elem->signal,
        const_names_get(being->properties->fsm->graph.names,
                        elem->new_state),
        elem->new_state,
        const_names_flags(being_f_names, being->flags),
        being->flags);

    frame = being->properties->fsm->state_frame_map[elem->new_state];
    being_set_action(being, frame);
}

void
being_set_fsm_state(struct being_t *being, int state_id)
{
    struct action_frame_t *frame;

    being->fsm_ctxt.state = state_id;

    frame = being->properties->fsm->state_frame_map[state_id];
    being_set_action(being, frame);
}

void
being_teleport(struct being_t *being, struct level_t *level, int x, int y)
{
    struct event_teleport_being_t *etb;
    struct event_t *event;

    if (level == NULL) {
        /* Teleport within level; just relocate being now. */
        being->phys.x = x;
        being->phys.y = y;
    } else {
        /* Inter-level teleport; schedule an immediate teleport event. */
        event = event_add(EVENT_TYPE_TELEPORT_BEING, world_tick);
        if (event == NULL) {
            return;
        }
        etb = &event->teleport_being;
        etb->being_id = being->entity.id;
        etb->level = level;
        etb->x = x;
        etb->y = y;
    }
}

static void
being_bleed_stop(struct being_t *being)
{
    being->flags &= ~BEING_F_BLEED;
    being_traits_remove_type(being, BEING_TRAITS_BLEED_CHANCE);
    being_traits_remove_type(being, BEING_TRAITS_BLEED_DEC);
}

static void
being_bleed_at(struct being_t *being, int worldx, int worldy)
{
    int duration;
    int chance;
    int roll;
    int dec;

    chance = being_traits_find_dflt(being, BEING_TRAITS_BLEED_CHANCE, 0);
    dec = being_traits_find_dflt(being, BEING_TRAITS_BLEED_DEC, 0);
    if (chance <= 0 || dec <= 0) {
        being_bleed_stop(being);
        return;
    }

    roll = rand_up_to(100);
    if (roll < chance) {
        duration = BEING_BLEED_FRAME_DUR_BASE +
                   rand_up_to(BEING_BLEED_FRAME_DUR_RAND);
        blood_stain_add(worldx, worldy, duration);
    }

    chance -= dec;
    if (chance <= 0) {
        being_bleed_stop(being);
        return;
    }

    being_traits_replace(being, BEING_TRAITS_BLEED_CHANCE, chance);
}

static void
being_crawl_update(struct being_t *being)
{
    int end;

    end = being_traits_find_dflt(being, BEING_TRAITS_CRAWL_END, 0);
    if (world_tick >= end) {
        being->flags &= ~BEING_F_CRAWL;
        being_traits_remove_type(being, BEING_TRAITS_CRAWL_END);
    }
}

void
being_bleed_dir(struct being_t *being, int direction)
{
    int *vect;
    int centerx;
    int centery;

    being_center(being, &centerx, &centery);
    vect = direction_to_vector(direction);
    centerx += vect[0] * WORLD_TILE_DIM;
    centery += vect[1] * WORLD_TILE_DIM;
    being_bleed_at(being, centerx, centery);
}

void
being_update(struct being_t *being)
{
    struct action_frame_t *frame;
    struct hit_box_desc_t *desc;
    struct hit_box_t hit_box;

    if (being->flags & BEING_F_TRASH) {
        being_erase(being);
        return;
    }

    if (!(being->flags & BEING_F_PLAYER) &&
        !zone_contains_being(being->phys.x, being->phys.y)) {

        return;
    }

#if 0
    if (being->ai_ctxt.scm_ctxt != NULL) {
        being_ai_update(being);
    }
#endif

    being_update_action(being);

    fsm_graph_update(&being->properties->fsm->graph, &being->fsm_ctxt,
                     being_fsm_cb, being);

    phys_update(&being->phys);

    if (being->flags & BEING_F_FRICTION) {
        phys_apply_friction(&being->phys, LIFE_FRICTION);
        if (being->phys.xspeed == 0 && being->phys.yspeed == 0) {
            being->flags &= ~BEING_F_FRICTION;
        }
    }

    fader_update(&being->fader);

    being_hit_box_terr(being);

    being->flags &= ~BEING_F_PER_TICK;

    if (being->flags & BEING_F_BLEED) {
        being_bleed_dir(being, DIRECTION_NONE);
    }
    if (being->flags & BEING_F_CRAWL) {
        being_crawl_update(being);
    }

    frame = being->action_player.frame;
    HIT_BOX_DESCS_FOREACH(frame->hit_boxes, desc) {
        hit_box_from_desc(&hit_box, desc, being->phys.x, being->phys.y);
        hit_box.owner = &being->entity;
        hit_box_add(&hit_box);
    }

    fsm_ctxt_update(&being->fsm_ctxt);
}

void
being_icon(struct being_t *being, struct icon_t *icon)
{
    // XXX
}

static void
being_draw_dot(struct being_t *being)
{
    struct being_traits_entry_t *traits;
    int canvasx;
    int canvasy;
    int radius;
    int worldx;
    int worldy;
    int color;

    traits = being_traits_find(being, BEING_TRAITS_PERIPH_RADIUS, NULL);
    if (traits != NULL) {
        radius = traits->val;
    } else {
        radius = 16;
    }

    being_center(being, &worldx, &worldy);
    world_view_canvas_loc(worldx, worldy, &canvasx, &canvasy);

    if (being->flags & BEING_F_PLAYER) {
        color = BEING_PERIPH_PLAYER_C2;
    } else {
        color = BEING_PERIPH_ENEMY_C2;
    }
    periph_dot_draw(world_view_info.canvas, canvasx, canvasy, radius, color,
                    being->angle);
}

static void
being_draw_periph(struct being_t *being)
{
    struct being_traits_entry_t *traits;
    int canvasx;
    int canvasy;
    int radius;
    int worldx;
    int worldy;
    int color;

    traits = being_traits_find(being, BEING_TRAITS_PERIPH_RADIUS, NULL);
    if (traits != NULL) {
        radius = traits->val;
    } else {
        radius = 16;
    }

    being_center(being, &worldx, &worldy);
    world_view_canvas_loc(worldx, worldy, &canvasx, &canvasy);

    if (being->flags & BEING_F_PLAYER) {
        color = BEING_PERIPH_PLAYER_C1;
    } else {
        color = BEING_PERIPH_ENEMY_C1;
    }
    periph_circ_draw(world_view_info.canvas, canvasx, canvasy, radius, color,
                     being->angle);
}

void
being_draw(struct being_t *being)
{
    struct BITMAP *bmp;

    bmp = image_bmp(being->image);

    world_view_smart_draw(bmp, being->phys.x, being->phys.y, &being->fader.color_info, 1.0, being->angle, 0, 0);
    if (being->flags & BEING_F_PERIPH) {
        being_draw_periph(being);
    }
    if (being->flags & BEING_F_DOT) {
        being_draw_dot(being);
    }
}

#include <stddef.h>
#include <math.h>
#include <assert.h>
#include "gen/gen_num.h"
#include "gen/gen_dbg.h"
#include "gen/direction.h"
#include "gfx/balleg.h"
#include "gfx/fader.h"
#include "life/action.h"
#include "life/data.h"
#include "life/collision.h"
#include "life/gun.h"
#include "life/world.h"
#include "life/event.h"
#include "life/life_defs.h"
#include "life/being.h"


/**
 * Fader settings that get applied on death.
 */
static struct fader_t being_death_fader = {
    .color_info = {
        .mode = COLOR_MODE_TRANS,
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255,
    },
    .delta = -10,
};

#define BEING_DEATH_FADE_DELAY (2 * LIFE_FPS)

#define BEING_DEATH_FADE_DURATION \
    (being_death_fader.color_info.a / -being_death_fader.delta + 1)

#define BEING_USE_RANGE             (2 * WORLD_TILE_DIM)

/*****************************************************************************
 * @being action effect                                                      *
 *****************************************************************************/

static void
being_set_speed_angle_abs(struct being_t *being, double angle_absolute,
                          int speed)
{
    double rads;

    rads = rads_normalize(angle_absolute);
    angle_to_vect(rads, speed, &being->phys.xspeed, &being->phys.yspeed);
}

static void
being_set_speed_angle_off(struct being_t *being, double angle_delta, int speed)
{
    being_set_speed_angle_abs(being, being->angle + angle_delta, speed);
}

static void
being_accelerate(struct being_t *being, int rate)
{
    double new_angle;
    int new_xspeed;
    int new_yspeed;
    int new_speed;
    int ratex;
    int ratey;

    angle_to_vect(being->angle, rate, &ratex, &ratey);

    new_xspeed = being->phys.xspeed + ratex;
    new_yspeed = being->phys.yspeed + ratey;
    new_angle = angle_from_vect(new_xspeed, new_yspeed);
    new_speed = distance(0, 0, new_xspeed, new_yspeed);

    if (new_speed > being->properties->max_speed) { 
        angle_to_vect(new_angle, being->properties->max_speed,
                      &new_xspeed, &new_yspeed);
    }

    being->phys.xspeed = new_xspeed;
    being->phys.yspeed = new_yspeed;
}

static void
being_adjust_angle(struct being_t *being, double angle_delta)
{
    being->angle = rads_normalize(being->angle + angle_delta);
}

static void
being_sched_death(struct being_t *being)
{
    struct event_t *event;

    /* Trash the being when fading is complete. */
    event = event_add(EVENT_TYPE_TRASH_BEING,
                      world_tick +
                      BEING_DEATH_FADE_DELAY + BEING_DEATH_FADE_DURATION);
    if (event == NULL) {
        /* Out of memory; just trash being immediately. */
        being->flags |= BEING_F_TRASH;
        return;
    }
    event->trash_being.being_id = being->entity.id;

    event = event_add(EVENT_TYPE_FADE_BEING,
                      world_tick + BEING_DEATH_FADE_DELAY);
    if (event != NULL) {
        event->fade_being.being_id = being->entity.id;
        event->fade_being.fader = &being_death_fader;
    }

    being->flags |= BEING_F_DEATH_SCHED;
}

static void
being_process_action_effect(struct being_t *being,
                            struct action_frame_t *frame, uint8_t effect)
{
    int direction;
    int speed;
    int angle;
    int dur;

    switch (effect) {
    case ACTION_EFFECT_MOVE_STOP:
        being->phys.xspeed = 0;
        being->phys.yspeed = 0;
        being->phys.xaccel = 0;
        being->phys.yaccel = 0;
        break;

    case ACTION_EFFECT_MOVE_U_S:
        being_accelerate(being, being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_U_F:
        being_accelerate(being, being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_UR_S:
        being_set_speed_angle_off(being, -45, being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_UR_F:
        being_set_speed_angle_off(being, -45, being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_R_S:
        being_set_speed_angle_off(being, -90, being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_R_F:
        being_set_speed_angle_off(being, -90, being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_DR_S:
        being_set_speed_angle_off(being, -135, being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_DR_F:
        being_set_speed_angle_off(being, -135, being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_D_S:
        being_set_speed_angle_off(being, -180, being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_D_F:
        being_set_speed_angle_off(being, -180, being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_DL_S:
        being_set_speed_angle_off(being, -225, being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_DL_F:
        being_set_speed_angle_off(being, -225, being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_L_S:
        being_set_speed_angle_off(being, -270, being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_L_F:
        being_set_speed_angle_off(being, -270, being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_UL_S:
        being_set_speed_angle_off(being, -315, being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_UL_F:
        being_set_speed_angle_off(being, -315, being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_C_U_S:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_UP),
                                  being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_C_U_F:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_UP),
                                  being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_C_UR_S:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_UR),
                                  being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_C_UR_F:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_UR),
                                  being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_C_R_S:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_RIGHT),
                                  being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_C_R_F:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_RIGHT),
                                  being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_C_DR_S:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_DR),
                                  being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_C_DR_F:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_DR),
                                  being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_C_D_S:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_DOWN),
                                  being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_C_D_F:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_DOWN),
                                  being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_C_DL_S:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_DL),
                                  being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_C_DL_F:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_DL),
                                  being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_C_L_S:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_LEFT),
                                  being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_C_L_F:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_LEFT),
                                  being->properties->run_speed);
        break;

    case ACTION_EFFECT_MOVE_C_UL_S:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_UL),
                                  being->properties->walk_speed);
        break;

    case ACTION_EFFECT_MOVE_C_UL_F:
        being_set_speed_angle_abs(being, direction_to_angle(DIRECTION_UL),
                                  being->properties->run_speed);
        break;

    case ACTION_EFFECT_TURN_CW_S:
        being_adjust_angle(being, -being->properties->turn_rate_slow);
        break;

    case ACTION_EFFECT_TURN_CW_F:
        being_adjust_angle(being, -being->properties->turn_rate_fast);
        break;

    case ACTION_EFFECT_TURN_CC_S:
        being_adjust_angle(being, being->properties->turn_rate_slow);
        break;

    case ACTION_EFFECT_TURN_CC_F:
        being_adjust_angle(being, being->properties->turn_rate_fast);
        break;

    case ACTION_EFFECT_SHOOT:
        combat_shoot(being);
        break;

    case ACTION_EFFECT_SHOOT_REL:
        being->gun.flags &= ~GUN_F_TRIGGER_HELD;
        break;

    case ACTION_EFFECT_REEL:
        speed = being_traits_remove_dflt(being, BEING_TRAITS_SPEED, INT_MIN);
        angle = being_traits_remove_dflt(being, BEING_TRAITS_ANGLE, INT_MIN);
        if (speed != INT_MIN && angle != INT_MIN) {
            being_set_speed_angle_abs(being, angle, speed);
        }
        break;

    case ACTION_EFFECT_FRICTION:
        being->flags |= BEING_F_FRICTION;
        break;

    case ACTION_EFFECT_SCHED_DEATH:
        if (!(being->flags & BEING_F_DEATH_SCHED)) {
            being_sched_death(being);
        }
        break;

    case ACTION_EFFECT_BLEED:
        being->flags |= BEING_F_BLEED;
        break;

    case ACTION_EFFECT_BOUNCE_H:
        if (being->flags & BEING_F_BLEED) {
            /* Bleed on the wall. */
            direction = direction_from_speed(being->phys.xspeed, 0);
            being_bleed_dir(being, direction);
        }
        phys_apply_friction(&being->phys, LIFE_BOUNCE_FRICTION);
        being->phys.xspeed *= -1;
        break;

    case ACTION_EFFECT_BOUNCE_V:
        if (being->flags & BEING_F_BLEED) {
            /* Bleed on the wall. */
            direction = direction_from_speed(0, being->phys.yspeed);
            being_bleed_dir(being, direction);
        }
        phys_apply_friction(&being->phys, LIFE_BOUNCE_FRICTION);
        being->phys.yspeed *= -1;
        break;

    case ACTION_EFFECT_TRASH:
        being->flags |= BEING_F_TRASH;
        break;

    case ACTION_EFFECT_NO_PERIPH:
        being->flags &= ~(BEING_F_PERIPH | BEING_F_DOT);
        break;

    case ACTION_EFFECT_CRAWL:
        dur = being_traits_remove_dflt(being, BEING_TRAITS_CRAWL_DUR, INT_MIN);
        if (dur != INT_MIN) {
            being_traits_replace(being, BEING_TRAITS_CRAWL_END,
                                 dur + world_tick);
            being->flags |= BEING_F_CRAWL;
        }
        break;

    default:
        assert(0);
        break;
    }
}

static void
being_process_action_frame(struct being_t *being,
                           struct action_frame_t *action_frame)
{
    int i;

    if (action_frame->effects != NULL) {
        for (i = 0; action_frame->effects[i] != -1; i++) {
            being_process_action_effect(being, action_frame,
                                        action_frame->effects[i]);
        }
    }
}

void
being_update_action(struct being_t *being)
{
    int rc;

    rc = action_player_update(&being->action_player);
    if (rc) {
        if (being->action_player.frame != NULL) {
            being_process_action_frame(being, being->action_player.frame);
        } else {
            fsm_ctxt_push_signal(&being->fsm_ctxt, FSM_SIG_ACTION_DONE);
        }
    }
}

void
being_signal(struct being_t *being, struct entity_t *sender, uint8_t signal)
{
    struct being_interest_entry_t *entry;
    struct being_interest_entry_t *prev;
    int fsm_sig;

    entry = being_interest_find(being, sender, BEING_PURPOSE_NONE,
                                NULL, &prev);
    if (entry == NULL) {
        return;
    }

    fsm_sig = FSM_SIG_NONE;
    switch (signal) {
    case ENTITY_SIGNAL_TRASH:
        being_interest_remove(being, entry, prev);
        break;

    default:
        break;
    }

    if (fsm_sig != FSM_SIG_NONE) {
        fsm_ctxt_push_signal(&being->fsm_ctxt, fsm_sig);
    }
}

static int
being_explicit_frame_dur(struct being_t *being, struct action_frame_t *frame)
{
    struct being_traits_entry_t *traits_prev;
    struct being_traits_entry_t *traits;
    int prev_dur;
    int cur_dur;

    switch (frame->duration) {
    case ACTION_FRAME_DURATION_INF:
        cur_dur = ACTION_FRAME_EXPLICIT_DURATION_NONE;
        break;

    case ACTION_FRAME_DURATION_VAR:
        traits = being_traits_find(being, BEING_TRAITS_FRAME_DUR,
                                   &traits_prev);
        if (traits != NULL) {
            cur_dur = traits->val;
            being_traits_remove(being, traits, traits_prev);
        } else {
            /* Continue where the previous frame left off.  This branch is
             * necessary for the case where duplicate signals get stacked, and
             * their corresponding frame dureation traits all replace one
             * another.  The first transition causes the trait to be deleted.
             * Reuse that variable duration for the subsequent transitions.
             */
            prev_dur = being->action_player.explicit_dur;
            if (prev_dur == ACTION_FRAME_EXPLICIT_DURATION_NONE) {
                cur_dur = 0;
            } else {
                cur_dur = prev_dur - being->action_player.tick;
                assert(cur_dur >= 0);
            }
        }
        break;

    default:
        assert(frame->duration > 0);
        cur_dur = ACTION_FRAME_EXPLICIT_DURATION_NONE;
        break;
    }

    return cur_dur;
}

void
being_set_action(struct being_t *being, struct action_frame_t *frame)
{
    int explicit_dur;
    int action_done;

    action_done = 0;

    /* Frames without an icon are just effects containers.  Only set the
     * current frame if it has an icon.
     */
    if (frame->icon != NULL) {
        explicit_dur = being_explicit_frame_dur(being, frame);
        if (explicit_dur == 0) {
            action_done = 1;
        }
        action_player_set_frame(&being->action_player, frame, explicit_dur);
    }

    being_process_action_frame(being, frame);

    /* If this action had a duration of 0, immediately transition to the next
     * action.
     */
    if (action_done) {
        being_update_action(being);
    }
}

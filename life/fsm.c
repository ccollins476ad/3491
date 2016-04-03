#include <stddef.h>
#include <string.h>
#include "life/fsm.h"

struct fsm_state_t fsm_state_null = {
    .elems = (struct fsm_state_elem_t[]) {
        { -1, -1 },
    },
};

struct fsm_state_elem_t *
fsm_state_elem_find(struct fsm_state_t *state, int signal)
{
    int i;

    for (i = 0; state->elems[i].signal != FSM_SIG_NONE; i++) {
        if (state->elems[i].signal == signal) {
            return state->elems + i;
        }
    }

    return NULL;
}

void
fsm_ctxt_update(struct fsm_ctxt_t *ctxt)
{
    ctxt->num_signals = 0;
}

void
fsm_ctxt_create(struct fsm_ctxt_t *ctxt)
{
    memset(ctxt, 0, sizeof *ctxt);
}

/**
 * @return          0 on success;
 *                  nonzero on error.
 */
int
fsm_ctxt_push_signal(struct fsm_ctxt_t *ctxt, int signal)
{
    if (ctxt->num_signals >= sizeof ctxt->signals / sizeof ctxt->signals[0]) {
        return -1;
    }

    ctxt->signals[ctxt->num_signals++] = signal;

    return 0;
}

/**
 * Processes pending signals in ascending order of index.  Terminates when a
 * signal causes a state transition or when no signals remain.
 *
 * @return          Signal-index if a signal causes the being's action to
 *                      change;
 *                  FSM_SIG_NONE on error.
 */
static int
fsm_graph_update_once(struct fsm_graph_t *graph, struct fsm_state_t *state,
                      struct fsm_ctxt_t *ctxt, fsm_state_transition_fn *cb,
                      void *arg)
{
    struct fsm_state_elem_t *elem;
    int elem_idx;
    int sig_idx;

    for (sig_idx = 0; sig_idx < ctxt->num_signals; ++sig_idx) {
        /* Skip signal if it has already been consumed. */
        if (ctxt->signals[sig_idx] == FSM_SIG_NONE) {
            continue;
        }

        /* Find the first transition which accepts this signal. */
        for (elem_idx = 0;
             state->elems[elem_idx].signal != FSM_SIG_NONE;
             ++elem_idx) {

            elem = state->elems + elem_idx;

            if (elem->signal == ctxt->signals[sig_idx]) {
                /* Consume signal. */
                ctxt->signals[sig_idx] = FSM_SIG_NONE;

                if (cb != NULL) {
                    cb(state, elem, arg);
                }

                if (!(graph->states[elem->new_state].flags & FSM_F_PSEUDO)) {
                    ctxt->state = elem->new_state;
                    return elem->signal;
                }
            }
        }
    }

    return FSM_SIG_NONE;
}

/**
 * Processes all pending signals in ascending order of index.
 */
void
fsm_graph_update(struct fsm_graph_t *graph, struct fsm_ctxt_t *ctxt,
                 fsm_state_transition_fn *cb, void *arg)
{
    struct fsm_state_t *state;
    int rc;

    /* Keep processing signals until the being's state stops changing. */
    do {
        state = &graph->states[ctxt->state];
        rc = fsm_graph_update_once(graph, state, ctxt, cb, arg);
    } while (rc != FSM_SIG_NONE);
}

int fsm_direction_bump_table[DIRECTION_MAX] = {
    [DIRECTION_UP]      = FSM_SIG_BUMP_TOP,
    [DIRECTION_UR]      = FSM_SIG_NONE,
    [DIRECTION_RIGHT]   = FSM_SIG_BUMP_RIGHT,
    [DIRECTION_DR]      = FSM_SIG_NONE,
    [DIRECTION_DOWN]    = FSM_SIG_BUMP_BOTTOM,
    [DIRECTION_DL]      = FSM_SIG_NONE,
    [DIRECTION_LEFT]    = FSM_SIG_BUMP_LEFT,
    [DIRECTION_UL]      = FSM_SIG_NONE,
};

int fsm_direction_run_table[DIRECTION_MAX] = {
    [DIRECTION_UP]      = FSM_SIG_INPUT_MOVE_U_F,
    [DIRECTION_UR]      = FSM_SIG_INPUT_MOVE_UR_F,
    [DIRECTION_RIGHT]   = FSM_SIG_INPUT_MOVE_R_F,
    [DIRECTION_DR]      = FSM_SIG_INPUT_MOVE_DR_F,
    [DIRECTION_DOWN]    = FSM_SIG_INPUT_MOVE_D_F,
    [DIRECTION_DL]      = FSM_SIG_INPUT_MOVE_DL_F,
    [DIRECTION_LEFT]    = FSM_SIG_INPUT_MOVE_L_F,
    [DIRECTION_UL]      = FSM_SIG_INPUT_MOVE_UL_F,
};

int fsm_direction_cardinal_fast_table[DIRECTION_MAX] = {
    [DIRECTION_UP]      = FSM_SIG_INPUT_MOVE_C_U_F,
    [DIRECTION_UR]      = FSM_SIG_INPUT_MOVE_C_UR_F,
    [DIRECTION_RIGHT]   = FSM_SIG_INPUT_MOVE_C_R_F,
    [DIRECTION_DR]      = FSM_SIG_INPUT_MOVE_C_DR_F,
    [DIRECTION_DOWN]    = FSM_SIG_INPUT_MOVE_C_D_F,
    [DIRECTION_DL]      = FSM_SIG_INPUT_MOVE_C_DL_F,
    [DIRECTION_LEFT]    = FSM_SIG_INPUT_MOVE_C_L_F,
    [DIRECTION_UL]      = FSM_SIG_INPUT_MOVE_C_UL_F,
};

int fsm_direction_cardinal_slow_table[DIRECTION_MAX] = {
    [DIRECTION_UP]      = FSM_SIG_INPUT_MOVE_C_U_S,
    [DIRECTION_UR]      = FSM_SIG_INPUT_MOVE_C_UR_S,
    [DIRECTION_RIGHT]   = FSM_SIG_INPUT_MOVE_C_R_S,
    [DIRECTION_DR]      = FSM_SIG_INPUT_MOVE_C_DR_S,
    [DIRECTION_DOWN]    = FSM_SIG_INPUT_MOVE_C_D_S,
    [DIRECTION_DL]      = FSM_SIG_INPUT_MOVE_C_DL_S,
    [DIRECTION_LEFT]    = FSM_SIG_INPUT_MOVE_C_L_S,
    [DIRECTION_UL]      = FSM_SIG_INPUT_MOVE_C_UL_S,
};

int *fsm_direction_cardinal_move_table[BEING_SPEED_MAX] = {
    [BEING_SPEED_SLOW]  = fsm_direction_cardinal_slow_table,
    [BEING_SPEED_FAST]  = fsm_direction_cardinal_fast_table,
};

int fsm_circular_turn_fast_table[CIRCULAR_MAX] = {
    [CIRCULAR_CW]       = FSM_SIG_INPUT_TURN_CW_F,
    [CIRCULAR_CCW]      = FSM_SIG_INPUT_TURN_CC_F,
};

int fsm_circular_turn_slow_table[CIRCULAR_MAX] = {
    [CIRCULAR_CW]       = FSM_SIG_INPUT_TURN_CW_S,
    [CIRCULAR_CCW]      = FSM_SIG_INPUT_TURN_CC_S,
};

int *fsm_circular_turn_table[BEING_SPEED_MAX] = {
    [BEING_SPEED_SLOW]  = fsm_circular_turn_slow_table,
    [BEING_SPEED_FAST]  = fsm_circular_turn_fast_table,
};

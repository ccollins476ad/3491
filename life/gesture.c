#include <assert.h>
#include <string.h>
#include "life/fsm.h"
#include "life/gesture.h"
#include "life/input.h"

static void
gesture_set_apply_group(struct gesture_set_t *gestures,
                        struct fsm_ctxt_t *ctxt,
                        struct gesture_fsm_mapping_t *elem_group)
{
    struct gesture_fsm_mapping_t *elem;
    int i;

    for (i = 0; elem_group[i].gesture_id != GESTURE_ID_NONE; ++i) {
        elem = elem_group + i;

        if (bit_map_get(&gestures->bit_map, elem->gesture_id)) {
            fsm_ctxt_push_signal(ctxt, elem->fsm_sig);
            break;
        }
    }
}

void
gesture_set_apply(struct gesture_set_t *gestures, struct fsm_ctxt_t *ctxt,
                  struct gesture_fsm_mapping_t **gesture_fsm_map)
{
    int i;

    for (i = 0; gesture_fsm_map[i] != NULL; i++) {
        gesture_set_apply_group(gestures, ctxt, gesture_fsm_map[i]);
    }
}

void
gesture_set_create(struct gesture_set_t *set)
{
    memset(set, 0, sizeof *set);
    set->bit_map.bits = set->buf;
    set->bit_map.num_bytes = sizeof set->buf;
    set->bit_map.num_bits = GESTURE_ID_MAX;
}

static int
gesture_match_input(int actual_push_state, int expected_push_state,
                    int match_op)
{
    switch (match_op) {
    case INPUT_SEQ_ELEM_MATCH_OP_EQUAL:
        return actual_push_state == expected_push_state;

    case INPUT_SEQ_ELEM_MATCH_OP_AND:
        return (expected_push_state & actual_push_state) ==
               expected_push_state;

    default:
        assert(0);
        return 0;
    }
}

static int
gesture_is_active(struct gesture_t *gesture)
{
    struct gesture_elem_t *elem;
    int expected_push_state;
    int actual_push_state;
    int i;

    /* Detect if each element is currently active.  If any are inactive, the
     * gesture is inactive.
     */
    for (i = gesture->num_elems - 1; i >= 0; --i) {
        elem = gesture->elems + i;

        expected_push_state = elem->push_state_code;
        actual_push_state = input_state_action_map[elem->input_id].code;
        if (!gesture_match_input(actual_push_state, expected_push_state,
                                 elem->match_op)) {
            return 0;
        }
    }

    return 1;
}

int
gesture_detect(struct gesture_t *input_gesture_map,
               struct gesture_set_t *gestures)
{
    struct gesture_t *gesture;
    int any_change;
    int i;

    memset(gestures->bit_map.bits, 0, gestures->bit_map.num_bytes);

    any_change = 0;
    for (i = 0; input_gesture_map[i].gesture_id != GESTURE_ID_NONE; ++i) {
        gesture = input_gesture_map + i;
        if (gesture_is_active(gesture)) {
            bit_map_set(&gestures->bit_map, gesture->gesture_id, 1);
            if (gesture->gesture_id != GESTURE_ID_NO_DIR) {
                any_change = 1;
            }
        }
    }

    return any_change;
}

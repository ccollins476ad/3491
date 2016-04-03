#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include "gen/gen_dbg.h"
#include "gfx/balleg.h"
#include "life/action_defs.h"
#include "life/gesture.h"
#include "life/world.h"
#include "life/input.h"

#define INPUT_TYPE_ACTION           0
#define INPUT_TYPE_MENU             1

struct input_data_t {
    uint8_t key_arr[32];
    int key_arr_sz;
};

static struct input_data_t input_data;

static void
input_kb_interrupt(int scancode)
{
    if (input_data.key_arr_sz <
        sizeof input_data.key_arr / sizeof input_data.key_arr[0]) {

        input_data.key_arr[input_data.key_arr_sz++] = scancode;
    }
} END_OF_FUNCTION(input_kb_interrupt)

static int
input_from_key(uint8_t k, int *out_input_type, int *out_input_id)
{
    *out_input_type = INPUT_TYPE_ACTION;
    switch (k & 0x7f) {
        case KEY_8_PAD: *out_input_id = INPUT_ACTION_ID_UP;         return 0;
        case KEY_6_PAD: *out_input_id = INPUT_ACTION_ID_RIGHT;      return 0;
        case KEY_2_PAD: *out_input_id = INPUT_ACTION_ID_DOWN;       return 0;
        case KEY_4_PAD: *out_input_id = INPUT_ACTION_ID_LEFT;       return 0;
        case KEY_K:     *out_input_id = INPUT_ACTION_ID_UP;         return 0;
        case KEY_L:     *out_input_id = INPUT_ACTION_ID_RIGHT;      return 0;
        case KEY_J:     *out_input_id = INPUT_ACTION_ID_DOWN;       return 0;
        case KEY_H:     *out_input_id = INPUT_ACTION_ID_LEFT;       return 0;
        case KEY_F:     *out_input_id = INPUT_ACTION_ID_SHOOT;      return 0;
        case KEY_S:     *out_input_id = INPUT_ACTION_ID_SLOW;       return 0;
        case KEY_D:     *out_input_id = INPUT_ACTION_ID_PROCEED;    return 0;
        case KEY_E:     *out_input_id = INPUT_ACTION_ID_CARD;       return 0;
        case KEY_SPACE: *out_input_id = INPUT_ACTION_ID_USE;        return 0;
    }

    *out_input_type = INPUT_TYPE_MENU;
    switch (k & 0x7f) {
        case KEY_ESC:   *out_input_id = INPUT_MENU_ID_QUIT;         return 0;
        case KEY_TAB:   *out_input_id = INPUT_MENU_ID_TIME_MODE;    return 0;
        case KEY_1:     *out_input_id = INPUT_MENU_ID_DEBUG_1;      return 0;
        case KEY_Q:     *out_input_id = INPUT_MENU_ID_DEBUG_2;      return 0;
        case KEY_3:     *out_input_id = INPUT_MENU_ID_DEBUG_3;      return 0;
        case KEY_4:     *out_input_id = INPUT_MENU_ID_DEBUG_4;      return 0;
        case KEY_W:     *out_input_id = INPUT_MENU_ID_DEBUG_5;      return 0;
    }

    return 1;
}

static void
input_process_key(uint8_t k)
{
    int input_type;
    int input_id;
    int on;
    int rc;

    /* Common case. */
    rc = input_from_key(k, &input_type, &input_id);
    if (rc != 0) {
        return;
    }

    if (!(k & 0x80)) {
        /* Press / hold. */
        on = 1;
    } else {
        /* Release. */
        on = 0;
    }

    switch (input_type) {
    case INPUT_TYPE_ACTION:
        input_state_event_action(input_id, on);
        break;

    case INPUT_TYPE_MENU:
        input_state_event_menu(input_id, on);
        break;

    default:
        assert(0);
        break;
    }
}

void
input_update(void)
{
    struct input_data_t input_clone;
    int i;

    /* Copy input data to local var and clear original to minimize
     * contention.
     */
    input_clone = input_data;
    input_data.key_arr_sz = 0;

    for (i = 0; i < input_clone.key_arr_sz; ++i) {
        input_process_key(input_clone.key_arr[i]);
    }
}

void
input_start(void)
{
    keyboard_lowlevel_callback = input_kb_interrupt;

    /* Disable keyboard repeat to get typewriter effect. */
    set_keyboard_rate(0, 0);
}

int
input_init(void)
{
    LOCK_FUNCTION(input_kb_interrupt);
    LOCK_VARIABLE(input_data.key_arr);
    LOCK_VARIABLE(input_data.key_arr_sz);

    return 0;
}


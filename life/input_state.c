#include <assert.h>
#include <string.h>
#include "gen/gen_dbg.h"
#include "life/gesture.h"
#include "life/input.h"

struct input_state_elem input_state_action_map[INPUT_ACTION_ID_MAX];
struct input_state_elem input_state_menu_map[INPUT_MENU_ID_MAX];

static void
input_state_event_gen(struct input_state_elem *map, int input_id, int on)
{
    struct input_state_elem *elem;

    elem = map + input_id;

    if (on) {
        elem->tapped = 1;
        elem->on = 1;
    } else {
        elem->on = 0;
    }
}

void
input_state_event_action(int input_id, int on)
{
    assert(input_id >= 0 && input_id < INPUT_ACTION_ID_MAX);
    input_state_event_gen(input_state_action_map, input_id, on);
}

void
input_state_event_menu(int input_id, int on)
{
    assert(input_id >= 0 && input_id < INPUT_MENU_ID_MAX);
    input_state_event_gen(input_state_menu_map, input_id, on);
}

static void
input_state_update_elem(struct input_state_elem *elem)
{
    if (elem->tapped) {
        elem->code = INPUT_CODE_TAP;
    } else if (elem->on) {
        elem->code = INPUT_CODE_HOLD;
    } else if (elem->prev_on) {
        elem->code = INPUT_CODE_RELEASE;
    } else {
        elem->code = 0;
    }

    elem->tapped = 0;
    elem->prev_on = elem->on;
}

static int
input_state_update_gen(struct input_state_elem *map, int map_size)
{
    int any_on;
    int i;

    any_on = 0;
    for (i = 0; i < map_size; i++) {
        input_state_update_elem(map + i);
        if (map[i].on) {
            any_on = 1;
        }
    }

    return any_on;
}

int
input_state_update_action(void)
{
    int any_on;

    any_on = input_state_update_gen(input_state_action_map,
                                    INPUT_ACTION_ID_MAX);
    return any_on;
}

void
input_state_update_menu(void)
{
    input_state_update_gen(input_state_menu_map, INPUT_MENU_ID_MAX);
}

void
input_state_clear(void)
{
    memset(&input_state_menu_map, 0, sizeof input_state_menu_map);
    memset(&input_state_action_map, 0, sizeof input_state_action_map);
}

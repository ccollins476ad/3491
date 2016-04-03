#ifndef INPUT_H_
#define INPUT_H_

#include <inttypes.h>
struct gesture_t;

#define INPUT_ID_NONE               (-1)
#define INPUT_ACTION_ID_UP          0
#define INPUT_ACTION_ID_RIGHT       1
#define INPUT_ACTION_ID_DOWN        2
#define INPUT_ACTION_ID_LEFT        3
#define INPUT_ACTION_ID_SLOW        4
#define INPUT_ACTION_ID_PROCEED     5
#define INPUT_ACTION_ID_CARD        6
#define INPUT_ACTION_ID_SHOOT       7
#define INPUT_ACTION_ID_USE         8
#define INPUT_ACTION_ID_MAX         9
#define INPUT_MENU_ID_QUIT          0
#define INPUT_MENU_ID_TIME_MODE     1
#define INPUT_MENU_ID_DEBUG_1       2
#define INPUT_MENU_ID_DEBUG_2       3
#define INPUT_MENU_ID_DEBUG_3       4
#define INPUT_MENU_ID_DEBUG_4       5
#define INPUT_MENU_ID_DEBUG_5       6
#define INPUT_MENU_ID_MAX           7

#define INPUT_F_PRESSED     0x01
#define INPUT_F_EDGE        0x02

#define INPUT_CODE_NONE     0
#define INPUT_CODE_TAP      (INPUT_F_PRESSED | INPUT_F_EDGE)
#define INPUT_CODE_HOLD     INPUT_F_PRESSED
#define INPUT_CODE_RELEASE  INPUT_F_EDGE

#define INPUT_SEQ_ELEM_MATCH_OP_EQUAL   '='
#define INPUT_SEQ_ELEM_MATCH_OP_AND     '&'

struct input_state_elem {
    uint8_t tapped;
    uint8_t on;
    uint8_t prev_on;
    uint8_t code;
};

void input_state_event_action(int input_id, int input_code);
void input_state_event_menu(int input_id, int input_code);
int input_state_update_action(void);
void input_state_update_menu(void);
void input_state_clear(void);

void input_update(void);
void input_start(void);
uint8_t input_get(int input_id);

int input_init(void);

extern struct input_state_elem input_state_action_map[INPUT_ACTION_ID_MAX];
extern struct input_state_elem input_state_menu_map[INPUT_MENU_ID_MAX];

#endif


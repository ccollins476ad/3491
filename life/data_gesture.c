#include <stddef.h>
#include "life/data.h"
#include "life/fsm.h"
#include "life/gesture.h"
#include "life/input.h"

/**
 * This array maps input sequences to gestures.
 */
struct gesture_t *data_gestures = (struct gesture_t[]) { {
    /* slow, card */
    .gesture_id = GESTURE_ID_MOVE_C_UR_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_DR_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_DL_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_UL_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_U_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_R_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_D_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_L_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    /* slow, proceed */
    .gesture_id = GESTURE_ID_MOVE_P_U_TURN_CW_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_U_TURN_CC_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_D_TURN_CW_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_D_TURN_CC_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_U_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_D_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_TURN_P_CW_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_TURN_P_CC_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    /* slow */
    .gesture_id = GESTURE_ID_MOVE_U_TURN_CW_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_U_TURN_CC_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_D_TURN_CW_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_D_TURN_CC_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_U_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_D_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_TURN_CW_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_TURN_CC_S,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_SLOW,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    /* card */
    .gesture_id = GESTURE_ID_MOVE_C_UR_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_UL_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_DR_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_DL_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_U_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_D_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_R_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_C_L_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_CARD,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    /* proceed */
    .gesture_id = GESTURE_ID_MOVE_P_U_TURN_CW_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_U_TURN_CC_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_D_TURN_CW_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_D_TURN_CC_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_U_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_P_D_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_TURN_P_CW_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_TURN_P_CC_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_PROCEED, INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    /* none */
    .gesture_id = GESTURE_ID_MOVE_U_TURN_CW_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_U_TURN_CC_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_D_TURN_CW_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_D_TURN_CC_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_U_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_MOVE_D_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_DOWN,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_TURN_CW_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_RIGHT,   INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_TURN_CC_F,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_LEFT,    INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_NO_DIR,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_UP,      INPUT_CODE_NONE,     '='},
        { INPUT_ACTION_ID_RIGHT,   INPUT_CODE_NONE,     '='},
        { INPUT_ACTION_ID_DOWN,    INPUT_CODE_NONE,     '='},
        { INPUT_ACTION_ID_LEFT,    INPUT_CODE_NONE,     '='},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_SHOOT,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_SHOOT,   INPUT_F_PRESSED,     '&'},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_SHOOT_REL,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_SHOOT,   INPUT_CODE_RELEASE,  '='},
        { -1 },
    },
}, {
    .gesture_id = GESTURE_ID_USE,
    .elems = (struct gesture_elem_t[]) {
        { INPUT_ACTION_ID_USE,     INPUT_CODE_TAP,      '='},
        { -1 },
    },
}, {
    .gesture_id = -1,
} };

/**
 * This array maps gestures to FSM signals.
 */
struct gesture_fsm_mapping_t **
data_gesture_fsm_map_basic = (struct gesture_fsm_mapping_t*[]) {
    (struct gesture_fsm_mapping_t[]) { {
        /* slow, card */
        .gesture_id = GESTURE_ID_MOVE_C_UR_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_UR_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_DR_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_DR_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_DL_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_DL_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_UL_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_UL_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_U_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_U_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_R_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_R_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_D_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_D_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_L_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_L_S,
        .and_flags = 0,
    }, {
        /* slow, proceed */
        .gesture_id = GESTURE_ID_MOVE_P_U_TURN_CW_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_U_TURN_CW_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_U_TURN_CC_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_U_TURN_CC_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_D_TURN_CW_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_D_TURN_CW_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_D_TURN_CC_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_D_TURN_CC_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_U_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_U_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_D_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_D_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_TURN_P_CW_S,
        .fsm_sig = FSM_SIG_INPUT_TURN_P_CW_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_TURN_P_CC_S,
        .fsm_sig = FSM_SIG_INPUT_TURN_P_CC_S,
        .and_flags = 0,
    }, {
        /* slow */
        .gesture_id = GESTURE_ID_MOVE_U_TURN_CW_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_U_TURN_CW_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_U_TURN_CC_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_U_TURN_CC_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_D_TURN_CW_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_D_TURN_CW_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_D_TURN_CC_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_D_TURN_CC_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_U_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_U_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_D_S,
        .fsm_sig = FSM_SIG_INPUT_MOVE_D_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_TURN_CW_S,
        .fsm_sig = FSM_SIG_INPUT_TURN_CW_S,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_TURN_CC_S,
        .fsm_sig = FSM_SIG_INPUT_TURN_CC_S,
        .and_flags = 0,
    }, {
        /* card */
        .gesture_id = GESTURE_ID_MOVE_C_UR_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_UR_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_DR_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_DR_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_DL_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_DL_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_UL_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_UL_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_U_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_U_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_R_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_R_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_D_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_D_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_C_L_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_C_L_F,
        .and_flags = 0,
    }, {
        /* proceed */
        .gesture_id = GESTURE_ID_MOVE_P_U_TURN_CW_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_U_TURN_CW_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_U_TURN_CC_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_U_TURN_CC_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_D_TURN_CW_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_D_TURN_CW_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_D_TURN_CC_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_D_TURN_CC_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_U_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_U_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_P_D_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_P_D_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_TURN_P_CW_F,
        .fsm_sig = FSM_SIG_INPUT_TURN_P_CW_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_TURN_P_CC_F,
        .fsm_sig = FSM_SIG_INPUT_TURN_P_CC_F,
        .and_flags = 0,
    }, {
        /* none */
        .gesture_id = GESTURE_ID_MOVE_U_TURN_CW_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_U_TURN_CW_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_U_TURN_CC_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_U_TURN_CC_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_D_TURN_CW_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_D_TURN_CW_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_D_TURN_CC_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_D_TURN_CC_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_U_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_U_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_MOVE_D_F,
        .fsm_sig = FSM_SIG_INPUT_MOVE_D_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_TURN_CW_F,
        .fsm_sig = FSM_SIG_INPUT_TURN_CW_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_TURN_CC_F,
        .fsm_sig = FSM_SIG_INPUT_TURN_CC_F,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_NO_DIR,
        .fsm_sig = FSM_SIG_INPUT_NO_DIR,
        .and_flags = 0,
    }, {
        .gesture_id = -1,
    } },

    (struct gesture_fsm_mapping_t[]) { {
        .gesture_id = GESTURE_ID_SHOOT,
        .fsm_sig = FSM_SIG_INPUT_SHOOT,
        .and_flags = 0,
    }, {
        .gesture_id = GESTURE_ID_SHOOT_REL,
        .fsm_sig = FSM_SIG_INPUT_SHOOT_REL,
        .and_flags = 0,
    }, {
        .gesture_id = -1,
    } },

    (struct gesture_fsm_mapping_t[]) { {
        .gesture_id = GESTURE_ID_USE,
        .fsm_sig = FSM_SIG_INPUT_USE,
        .and_flags = 0,
    }, {
        .gesture_id = -1,
    } },

    NULL,
};

void
data_gesture_init(void)
{
    struct gesture_t *gesture;
    int g;
    int e;

    for (g = 0; data_gestures[g].gesture_id != -1; g++) {
        gesture = data_gestures + g;

        gesture->num_elems = 0;
        for (e = 0; gesture->elems[e].input_id != -1; e++) {
            gesture->num_elems++;
        }
    }
}

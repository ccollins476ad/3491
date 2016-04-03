#ifndef FSM_H_
#define FSM_H_

#include <inttypes.h>
#include "gen/direction.h"

#define FSM_SIG_NONE                    (-1)
#define FSM_SIG_INPUT_NO_DIR            0
#define FSM_SIG_INPUT_TURN_CW_S         1
#define FSM_SIG_INPUT_TURN_CW_F         2
#define FSM_SIG_INPUT_TURN_CC_S         3
#define FSM_SIG_INPUT_TURN_CC_F         4
#define FSM_SIG_INPUT_MOVE_U_S          5
#define FSM_SIG_INPUT_MOVE_U_F          6
#define FSM_SIG_INPUT_MOVE_UR_S         7
#define FSM_SIG_INPUT_MOVE_UR_F         8
#define FSM_SIG_INPUT_MOVE_R_S          9
#define FSM_SIG_INPUT_MOVE_R_F          10
#define FSM_SIG_INPUT_MOVE_DR_S         11
#define FSM_SIG_INPUT_MOVE_DR_F         12
#define FSM_SIG_INPUT_MOVE_D_S          13
#define FSM_SIG_INPUT_MOVE_D_F          14
#define FSM_SIG_INPUT_MOVE_DL_S         15
#define FSM_SIG_INPUT_MOVE_DL_F         16
#define FSM_SIG_INPUT_MOVE_L_S          17
#define FSM_SIG_INPUT_MOVE_L_F          18
#define FSM_SIG_INPUT_MOVE_UL_S         19
#define FSM_SIG_INPUT_MOVE_UL_F         20
#define FSM_SIG_INPUT_MOVE_U_TURN_CW_S  21
#define FSM_SIG_INPUT_MOVE_U_TURN_CW_F  22
#define FSM_SIG_INPUT_MOVE_U_TURN_CC_S  23
#define FSM_SIG_INPUT_MOVE_U_TURN_CC_F  24
#define FSM_SIG_INPUT_MOVE_D_TURN_CW_S  25
#define FSM_SIG_INPUT_MOVE_D_TURN_CW_F  26
#define FSM_SIG_INPUT_MOVE_D_TURN_CC_S  27
#define FSM_SIG_INPUT_MOVE_D_TURN_CC_F  28
#define FSM_SIG_INPUT_MOVE_C_U_S        29
#define FSM_SIG_INPUT_MOVE_C_U_F        30
#define FSM_SIG_INPUT_MOVE_C_UR_S       31
#define FSM_SIG_INPUT_MOVE_C_UR_F       32
#define FSM_SIG_INPUT_MOVE_C_R_S        33
#define FSM_SIG_INPUT_MOVE_C_R_F        34
#define FSM_SIG_INPUT_MOVE_C_DR_S       35
#define FSM_SIG_INPUT_MOVE_C_DR_F       36
#define FSM_SIG_INPUT_MOVE_C_D_S        37
#define FSM_SIG_INPUT_MOVE_C_D_F        38
#define FSM_SIG_INPUT_MOVE_C_DL_S       39
#define FSM_SIG_INPUT_MOVE_C_DL_F       40
#define FSM_SIG_INPUT_MOVE_C_L_S        41
#define FSM_SIG_INPUT_MOVE_C_L_F        42
#define FSM_SIG_INPUT_MOVE_C_UL_S       43
#define FSM_SIG_INPUT_MOVE_C_UL_F       44
#define FSM_SIG_INPUT_TURN_P_CW_S       45
#define FSM_SIG_INPUT_TURN_P_CW_F       46
#define FSM_SIG_INPUT_TURN_P_CC_S       47
#define FSM_SIG_INPUT_TURN_P_CC_F       48
#define FSM_SIG_INPUT_MOVE_P_U_S        49
#define FSM_SIG_INPUT_MOVE_P_U_F        50
#define FSM_SIG_INPUT_MOVE_P_D_S        51
#define FSM_SIG_INPUT_MOVE_P_D_F        52
#define FSM_SIG_INPUT_MOVE_P_U_TURN_CW_S 53
#define FSM_SIG_INPUT_MOVE_P_U_TURN_CW_F 54
#define FSM_SIG_INPUT_MOVE_P_U_TURN_CC_S 55
#define FSM_SIG_INPUT_MOVE_P_U_TURN_CC_F 56
#define FSM_SIG_INPUT_MOVE_P_D_TURN_CW_S 57
#define FSM_SIG_INPUT_MOVE_P_D_TURN_CW_F 58
#define FSM_SIG_INPUT_MOVE_P_D_TURN_CC_S 59
#define FSM_SIG_INPUT_MOVE_P_D_TURN_CC_F 60
#define FSM_SIG_INPUT_SHOOT             61
#define FSM_SIG_INPUT_SHOOT_REL         62
#define FSM_SIG_INPUT_USE               64
#define FSM_SIG_STRIKE                  65
#define FSM_SIG_ACTION_DONE             66
#define FSM_SIG_DIE                     67
#define FSM_SIG_BUMP_TOP                68
#define FSM_SIG_BUMP_RIGHT              69
#define FSM_SIG_BUMP_BOTTOM             70
#define FSM_SIG_BUMP_LEFT               71
#define FSM_SIG_ANNIHILATE              72
#define FSM_SIG_PEACEFUL_DEATH          73
#define FSM_SIG_MAX                     74

#define FSM_STATE_NONE                  (-1)
#define FSM_STATE_STAND                 0
#define FSM_STATE_CUSTOM_START          1

#define FSM_F_PSEUDO                    0x01

#define FSM_CTXT_MAX_SIGNALS            16

struct fsm_state_elem_t {
    int signal;
    int new_state;
};

struct fsm_state_t {
    struct fsm_state_elem_t *elems;
    uint8_t flags;
};

struct fsm_graph_t {
    struct fsm_state_t *states;
    int num_states;

    struct const_name_pair_t *names;
};

struct fsm_ctxt_t {
    int signals[FSM_CTXT_MAX_SIGNALS];
    int num_signals;
    int state;
    uint8_t flags;
};

extern struct fsm_state_t fsm_state_null;

struct fsm_state_elem_t *fsm_state_elem_find(struct fsm_state_t *state,
                                             int signal);

void fsm_ctxt_update_input(struct fsm_ctxt_t *ctxt);
void fsm_ctxt_update(struct fsm_ctxt_t *ctxt);
void fsm_ctxt_create(struct fsm_ctxt_t *ctxt);
int fsm_ctxt_push_signal(struct fsm_ctxt_t *ctxt, int signal);

typedef void fsm_state_transition_fn(struct fsm_state_t *state,
                                 struct fsm_state_elem_t *elem,
                                 void *arg);
void fsm_graph_update(struct fsm_graph_t *graph, struct fsm_ctxt_t *ctxt,
                      fsm_state_transition_fn *cb, void *arg);

#if 0
extern int fsm_direction_bump_table[DIRECTION_MAX];
extern int fsm_direction_run_table[DIRECTION_MAX];
extern int fsm_direction_cardinal_fast_table[DIRECTION_MAX];
extern int fsm_direction_cardinal_slow_table[DIRECTION_MAX];
extern int *fsm_direction_cardinal_move_table[BEING_SPEED_MAX];

extern int fsm_circular_turn_fast_table[CIRCULAR_MAX];
extern int fsm_circular_turn_slow_table[CIRCULAR_MAX];
extern int *fsm_circular_turn_table[BEING_SPEED_MAX];
#endif

#endif 


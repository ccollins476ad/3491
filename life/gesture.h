#ifndef GESTURE_H_
#define GESTURE_H_

#include <inttypes.h>
#include "gen/container.h"
struct bit_map_t;
struct fsm_ctxt_t;

#define GESTURE_ID_NONE                 (-1)
#define GESTURE_ID_NO_DIR               0
#define GESTURE_ID_TURN_CW_S            1
#define GESTURE_ID_TURN_CW_F            2
#define GESTURE_ID_TURN_CC_S            3
#define GESTURE_ID_TURN_CC_F            4
#define GESTURE_ID_MOVE_U_S             5
#define GESTURE_ID_MOVE_U_F             6
#define GESTURE_ID_MOVE_D_S             7
#define GESTURE_ID_MOVE_D_F             8
#define GESTURE_ID_MOVE_U_TURN_CW_S     9
#define GESTURE_ID_MOVE_U_TURN_CW_F     10
#define GESTURE_ID_MOVE_U_TURN_CC_S     11
#define GESTURE_ID_MOVE_U_TURN_CC_F     12
#define GESTURE_ID_MOVE_D_TURN_CW_S     13
#define GESTURE_ID_MOVE_D_TURN_CW_F     14
#define GESTURE_ID_MOVE_D_TURN_CC_S     15
#define GESTURE_ID_MOVE_D_TURN_CC_F     16
#define GESTURE_ID_TURN_P_CW_S          17
#define GESTURE_ID_TURN_P_CW_F          18
#define GESTURE_ID_TURN_P_CC_S          19
#define GESTURE_ID_TURN_P_CC_F          20
#define GESTURE_ID_MOVE_P_U_S           21
#define GESTURE_ID_MOVE_P_U_F           22
#define GESTURE_ID_MOVE_P_D_S           23
#define GESTURE_ID_MOVE_P_D_F           24
#define GESTURE_ID_MOVE_P_U_TURN_CW_S   25
#define GESTURE_ID_MOVE_P_U_TURN_CW_F   26
#define GESTURE_ID_MOVE_P_U_TURN_CC_S   27
#define GESTURE_ID_MOVE_P_U_TURN_CC_F   28
#define GESTURE_ID_MOVE_P_D_TURN_CW_S   29
#define GESTURE_ID_MOVE_P_D_TURN_CW_F   30
#define GESTURE_ID_MOVE_P_D_TURN_CC_S   31
#define GESTURE_ID_MOVE_P_D_TURN_CC_F   32
#define GESTURE_ID_MOVE_C_U_F           33
#define GESTURE_ID_MOVE_C_U_S           34
#define GESTURE_ID_MOVE_C_UR_F          35
#define GESTURE_ID_MOVE_C_UR_S          36
#define GESTURE_ID_MOVE_C_R_F           37
#define GESTURE_ID_MOVE_C_R_S           38
#define GESTURE_ID_MOVE_C_DR_F          39
#define GESTURE_ID_MOVE_C_DR_S          40
#define GESTURE_ID_MOVE_C_D_F           41
#define GESTURE_ID_MOVE_C_D_S           42
#define GESTURE_ID_MOVE_C_DL_F          43
#define GESTURE_ID_MOVE_C_DL_S          44
#define GESTURE_ID_MOVE_C_L_F           45
#define GESTURE_ID_MOVE_C_L_S           46
#define GESTURE_ID_MOVE_C_UL_F          47
#define GESTURE_ID_MOVE_C_UL_S          48
#define GESTURE_ID_SHOOT                49
#define GESTURE_ID_SHOOT_REL            50
#define GESTURE_ID_USE                  51
#define GESTURE_ID_MAX                  52

struct gesture_set_t {
    struct bit_map_t bit_map;
    unsigned char buf[BIT_MAP_BUFSZ(GESTURE_ID_MAX)];
};

struct gesture_elem_t {
    int input_id;
    uint8_t push_state_code;
    uint8_t match_op;
};

struct gesture_t {
    struct gesture_elem_t *elems;
    int gesture_id;
    int num_elems; /* Filled in dynamically. */
};

struct gesture_fsm_mapping_t {
    int gesture_id;
    int fsm_sig;
    uint8_t and_flags;
};

void gesture_set_create(struct gesture_set_t *set);

void gesture_set_apply(struct gesture_set_t *gestures,
                       struct fsm_ctxt_t *ctxt,
                       struct gesture_fsm_mapping_t **gesture_fsm_map);

int gesture_detect(struct gesture_t *input_gesture_map,
                       struct gesture_set_t *gestures);

#endif

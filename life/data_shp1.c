#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "gfx/gfx.h"
#include "gfx/tile.h"
#include "life/action.h"
#include "life/being.h"
#include "life/generated/const_names.h"
#include "life/hit_box.h"
#include "life/world.h"
#include "life/data.h"
#include "life/data_shp1.h"

static struct hit_box_desc_t data_shp1_hit_boxes_stand[] = { {
    .flags = HIT_BOX_F_CLP | HIT_BOX_F_DEF,
    .shape = {
        .type = HIT_BOX_TYPE_RECTANGLE,
        .rectangle = { 8, 8, 48, 48, }
    },
},
    HIT_BOX_DESC_NONE,
};

static struct icon_t data_shp1_icon = {
    TILE_ID_FACE_SOLID, COLOR_NONE
};
static struct icon_t data_shp1_icon_dead = {
    TILE_ID_FACE_HOLLOW, COLOR_NONE
};

static struct action_frame_t
af[DATA_SHP1_AF_MAX] = {
    [DATA_SHP1_AF_STAND] = {
        .icon = &data_shp1_icon,
        .hit_boxes = data_shp1_hit_boxes_stand,
        .duration = ACTION_FRAME_DURATION_INF,
    },
    [DATA_SHP1_AF_TURN_CW_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_TURN_CW_S,
            -1,
        },
    },
    [DATA_SHP1_AF_TURN_CW_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_TURN_CW_F,
            -1,
        },
    },
    [DATA_SHP1_AF_TURN_CC_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_TURN_CC_S,
            -1,
        },
    },
    [DATA_SHP1_AF_TURN_CC_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_TURN_CC_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_U_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_U_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_U_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_U_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_UR_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_UR_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_UR_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_UR_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_R_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_R_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_R_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_R_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_DR_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_DR_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_DR_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_DR_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_D_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_D_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_D_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_D_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_DL_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_DL_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_DL_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_DL_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_L_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_L_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_L_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_L_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_UL_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_UL_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_UL_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_UL_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_U_TURN_CW_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_U_S,
            ACTION_EFFECT_TURN_CW_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_U_TURN_CW_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_U_F,
            ACTION_EFFECT_TURN_CW_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_U_TURN_CC_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_U_S,
            ACTION_EFFECT_TURN_CC_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_U_TURN_CC_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_U_F,
            ACTION_EFFECT_TURN_CC_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_D_TURN_CW_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_D_S,
            ACTION_EFFECT_TURN_CW_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_D_TURN_CW_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_D_F,
            ACTION_EFFECT_TURN_CW_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_D_TURN_CC_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_D_S,
            ACTION_EFFECT_TURN_CC_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_D_TURN_CC_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_D_F,
            ACTION_EFFECT_TURN_CC_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_U_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_U_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_U_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_U_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_UR_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_UR_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_UR_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_UR_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_R_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_R_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_R_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_R_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_DR_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_DR_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_DR_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_DR_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_D_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_D_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_D_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_D_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_DL_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_DL_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_DL_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_DL_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_L_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_L_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_L_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_L_F,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_UL_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_UL_S,
            -1,
        },
    },
    [DATA_SHP1_AF_MOVE_C_UL_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_MOVE_C_UL_F,
            -1,
        },
    },
    [DATA_SHP1_AF_TURN_P_CW_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_TURN_CW_S,
            -1,
        },
    },
    [DATA_SHP1_AF_TURN_P_CW_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_TURN_CW_F,
            -1,
        },
    },
    [DATA_SHP1_AF_TURN_P_CC_S] = {
        .effects = (int[]) {
            ACTION_EFFECT_TURN_CC_S,
            -1,
        },
    },
    [DATA_SHP1_AF_TURN_P_CC_F] = {
        .effects = (int[]) {
            ACTION_EFFECT_TURN_CC_F,
            -1,
        },
    },
    [DATA_SHP1_AF_BOUNCE_H] = {
        .effects = (int[]) {
            ACTION_EFFECT_BOUNCE_H,
            -1,
        },
    },
    [DATA_SHP1_AF_BOUNCE_V] = {
        .effects = (int[]) {
            ACTION_EFFECT_BOUNCE_V,
            -1,
        },
    },
    [DATA_SHP1_AF_SHOOT] = {
        .effects = (int[]) {
            ACTION_EFFECT_SHOOT,
            -1,
        },
    },
    [DATA_SHP1_AF_SHOOT_REL] = {
        .effects = (int[]) {
            ACTION_EFFECT_SHOOT_REL,
            -1,
        },
    },
    [DATA_SHP1_AF_USE] = {
        .effects = (int[]) {
            ACTION_EFFECT_USE,
            -1,
        },
    },
    [DATA_SHP1_AF_REEL] = {
        .icon = &data_shp1_icon,
        .hit_boxes = data_shp1_hit_boxes_stand,
        .effects = (int[]) {
            ACTION_EFFECT_REEL,
            ACTION_EFFECT_CRAWL,
            ACTION_EFFECT_BLEED,
            ACTION_EFFECT_FRICTION,
            -1,
        },
        .duration = ACTION_FRAME_DURATION_VAR,
    },
    [DATA_SHP1_AF_DEAD_IDLE] = {
        .icon = &data_shp1_icon_dead,
        .hit_boxes = data_shp1_hit_boxes_stand,
        .effects = (int[]) {
            ACTION_EFFECT_REEL,
            ACTION_EFFECT_BLEED,
            ACTION_EFFECT_FRICTION,
            ACTION_EFFECT_NO_PERIPH,
            -1,
        },
        .duration = ACTION_FRAME_DURATION_INF,
    },
    [DATA_SHP1_AF_DEAD_REEL] = {
        .icon = &data_shp1_icon_dead,
        .hit_boxes = data_shp1_hit_boxes_stand,
        .effects = (int[]) {
            ACTION_EFFECT_SCHED_DEATH,
            ACTION_EFFECT_REEL,
            ACTION_EFFECT_BLEED,
            ACTION_EFFECT_FRICTION,
            ACTION_EFFECT_NO_PERIPH,
            -1,
        },
        .duration = ACTION_FRAME_DURATION_VAR,
    },
    [DATA_SHP1_AF_ANNIHILATE] = {
        .effects = (int[]) {
            ACTION_EFFECT_REEL,
            ACTION_EFFECT_TRASH,
            -1,
        },
    },
};

struct being_fsm_t data_shp1_being_fsm = {
    .state_frame_map = (struct action_frame_t*[DATA_SHP1_AS_MAX]) {
        [DATA_SHP1_AS_STAND] =              af + DATA_SHP1_AF_STAND,
        [DATA_SHP1_AS_TURN_CW_S] =          af + DATA_SHP1_AF_TURN_CW_S,
        [DATA_SHP1_AS_TURN_CW_F] =          af + DATA_SHP1_AF_TURN_CW_F,
        [DATA_SHP1_AS_TURN_CC_S] =          af + DATA_SHP1_AF_TURN_CC_S,
        [DATA_SHP1_AS_TURN_CC_F] =          af + DATA_SHP1_AF_TURN_CC_F,
        [DATA_SHP1_AS_MOVE_U_S] =           af + DATA_SHP1_AF_MOVE_U_S,
        [DATA_SHP1_AS_MOVE_U_F] =           af + DATA_SHP1_AF_MOVE_U_F,
        [DATA_SHP1_AS_MOVE_UR_S] =          af + DATA_SHP1_AF_MOVE_UR_S,
        [DATA_SHP1_AS_MOVE_UR_F] =          af + DATA_SHP1_AF_MOVE_UR_F,
        [DATA_SHP1_AS_MOVE_R_S] =           af + DATA_SHP1_AF_MOVE_R_S,
        [DATA_SHP1_AS_MOVE_R_F] =           af + DATA_SHP1_AF_MOVE_R_F,
        [DATA_SHP1_AS_MOVE_DR_S] =          af + DATA_SHP1_AF_MOVE_DR_S,
        [DATA_SHP1_AS_MOVE_DR_F] =          af + DATA_SHP1_AF_MOVE_DR_F,
        [DATA_SHP1_AS_MOVE_D_S] =           af + DATA_SHP1_AF_MOVE_D_S,
        [DATA_SHP1_AS_MOVE_D_F] =           af + DATA_SHP1_AF_MOVE_D_F,
        [DATA_SHP1_AS_MOVE_DL_S] =          af + DATA_SHP1_AF_MOVE_DL_S,
        [DATA_SHP1_AS_MOVE_DL_F] =          af + DATA_SHP1_AF_MOVE_DL_F,
        [DATA_SHP1_AS_MOVE_L_S] =           af + DATA_SHP1_AF_MOVE_L_S,
        [DATA_SHP1_AS_MOVE_L_F] =           af + DATA_SHP1_AF_MOVE_L_F,
        [DATA_SHP1_AS_MOVE_UL_S] =          af + DATA_SHP1_AF_MOVE_UL_S,
        [DATA_SHP1_AS_MOVE_UL_F] =          af + DATA_SHP1_AF_MOVE_UL_F,
        [DATA_SHP1_AS_MOVE_U_TURN_CW_S] =   af + DATA_SHP1_AF_MOVE_U_TURN_CW_S,
        [DATA_SHP1_AS_MOVE_U_TURN_CW_F] =   af + DATA_SHP1_AF_MOVE_U_TURN_CW_F,
        [DATA_SHP1_AS_MOVE_U_TURN_CC_S] =   af + DATA_SHP1_AF_MOVE_U_TURN_CC_S,
        [DATA_SHP1_AS_MOVE_U_TURN_CC_F] =   af + DATA_SHP1_AF_MOVE_U_TURN_CC_F,
        [DATA_SHP1_AS_MOVE_D_TURN_CW_S] =   af + DATA_SHP1_AF_MOVE_D_TURN_CW_S,
        [DATA_SHP1_AS_MOVE_D_TURN_CW_F] =   af + DATA_SHP1_AF_MOVE_D_TURN_CW_F,
        [DATA_SHP1_AS_MOVE_D_TURN_CC_S] =   af + DATA_SHP1_AF_MOVE_D_TURN_CC_S,
        [DATA_SHP1_AS_MOVE_D_TURN_CC_F] =   af + DATA_SHP1_AF_MOVE_D_TURN_CC_F,
        [DATA_SHP1_AS_MOVE_C_U_S] =         af + DATA_SHP1_AF_MOVE_C_U_S,
        [DATA_SHP1_AS_MOVE_C_U_F] =         af + DATA_SHP1_AF_MOVE_C_U_F,
        [DATA_SHP1_AS_MOVE_C_UR_S] =        af + DATA_SHP1_AF_MOVE_C_UR_S,
        [DATA_SHP1_AS_MOVE_C_UR_F] =        af + DATA_SHP1_AF_MOVE_C_UR_F,
        [DATA_SHP1_AS_MOVE_C_R_S] =         af + DATA_SHP1_AF_MOVE_C_R_S,
        [DATA_SHP1_AS_MOVE_C_R_F] =         af + DATA_SHP1_AF_MOVE_C_R_F,
        [DATA_SHP1_AS_MOVE_C_DR_S] =        af + DATA_SHP1_AF_MOVE_C_DR_S,
        [DATA_SHP1_AS_MOVE_C_DR_F] =        af + DATA_SHP1_AF_MOVE_C_DR_F,
        [DATA_SHP1_AS_MOVE_C_D_S] =         af + DATA_SHP1_AF_MOVE_C_D_S,
        [DATA_SHP1_AS_MOVE_C_D_F] =         af + DATA_SHP1_AF_MOVE_C_D_F,
        [DATA_SHP1_AS_MOVE_C_DL_S] =        af + DATA_SHP1_AF_MOVE_C_DL_S,
        [DATA_SHP1_AS_MOVE_C_DL_F] =        af + DATA_SHP1_AF_MOVE_C_DL_F,
        [DATA_SHP1_AS_MOVE_C_L_S] =         af + DATA_SHP1_AF_MOVE_C_L_S,
        [DATA_SHP1_AS_MOVE_C_L_F] =         af + DATA_SHP1_AF_MOVE_C_L_F,
        [DATA_SHP1_AS_MOVE_C_UL_S] =        af + DATA_SHP1_AF_MOVE_C_UL_S,
        [DATA_SHP1_AS_MOVE_C_UL_F] =        af + DATA_SHP1_AF_MOVE_C_UL_F,
        [DATA_SHP1_AS_BOUNCE_H] =           af + DATA_SHP1_AF_BOUNCE_H,
        [DATA_SHP1_AS_BOUNCE_V] =           af + DATA_SHP1_AF_BOUNCE_V,
        [DATA_SHP1_AS_TURN_P_CW_S] =        af + DATA_SHP1_AF_TURN_P_CW_S,
        [DATA_SHP1_AS_TURN_P_CW_F] =        af + DATA_SHP1_AF_TURN_P_CW_F,
        [DATA_SHP1_AS_TURN_P_CC_S] =        af + DATA_SHP1_AF_TURN_P_CC_S,
        [DATA_SHP1_AS_TURN_P_CC_F] =        af + DATA_SHP1_AF_TURN_P_CC_F,
        [DATA_SHP1_AS_SHOOT] =              af + DATA_SHP1_AF_SHOOT,
        [DATA_SHP1_AS_SHOOT_REL] =          af + DATA_SHP1_AF_SHOOT_REL,
        [DATA_SHP1_AS_USE] =                af + DATA_SHP1_AF_USE,
        [DATA_SHP1_AS_REEL] =               af + DATA_SHP1_AF_REEL,
        [DATA_SHP1_AS_DEAD_IDLE] =          af + DATA_SHP1_AF_DEAD_IDLE,
        [DATA_SHP1_AS_DEAD_REEL] =          af + DATA_SHP1_AF_DEAD_REEL,
        [DATA_SHP1_AS_ANNIHILATE] =         af + DATA_SHP1_AF_ANNIHILATE,
    },

    .graph = {
        .states = (struct fsm_state_t[DATA_SHP1_AS_MAX]) {
            [DATA_SHP1_AS_STAND] = {
                .elems = (struct fsm_state_elem_t[]) {
                    { FSM_SIG_INPUT_NO_DIR, DATA_SHP1_AS_STAND },
                    { FSM_SIG_INPUT_TURN_CW_S, DATA_SHP1_AS_TURN_CW_S },
                    { FSM_SIG_INPUT_TURN_CW_F, DATA_SHP1_AS_TURN_CW_F },
                    { FSM_SIG_INPUT_TURN_CC_S, DATA_SHP1_AS_TURN_CC_S },
                    { FSM_SIG_INPUT_TURN_CC_F, DATA_SHP1_AS_TURN_CC_F },
                    { FSM_SIG_INPUT_MOVE_U_S, DATA_SHP1_AS_MOVE_U_S },
                    { FSM_SIG_INPUT_MOVE_U_F, DATA_SHP1_AS_MOVE_U_F },
                    { FSM_SIG_INPUT_MOVE_UR_S, DATA_SHP1_AS_MOVE_UR_S },
                    { FSM_SIG_INPUT_MOVE_UR_F, DATA_SHP1_AS_MOVE_UR_F },
                    { FSM_SIG_INPUT_MOVE_R_S, DATA_SHP1_AS_MOVE_R_S },
                    { FSM_SIG_INPUT_MOVE_R_F, DATA_SHP1_AS_MOVE_R_F },
                    { FSM_SIG_INPUT_MOVE_DR_S, DATA_SHP1_AS_MOVE_DR_S },
                    { FSM_SIG_INPUT_MOVE_DR_F, DATA_SHP1_AS_MOVE_DR_F },
                    { FSM_SIG_INPUT_MOVE_D_S, DATA_SHP1_AS_MOVE_D_S },
                    { FSM_SIG_INPUT_MOVE_D_F, DATA_SHP1_AS_MOVE_D_F },
                    { FSM_SIG_INPUT_MOVE_DL_S, DATA_SHP1_AS_MOVE_DL_S },
                    { FSM_SIG_INPUT_MOVE_DL_F, DATA_SHP1_AS_MOVE_DL_F },
                    { FSM_SIG_INPUT_MOVE_L_S, DATA_SHP1_AS_MOVE_L_S },
                    { FSM_SIG_INPUT_MOVE_L_F, DATA_SHP1_AS_MOVE_L_F },
                    { FSM_SIG_INPUT_MOVE_UL_S, DATA_SHP1_AS_MOVE_UL_S },
                    { FSM_SIG_INPUT_MOVE_UL_F, DATA_SHP1_AS_MOVE_UL_F },
                    { FSM_SIG_INPUT_MOVE_U_TURN_CW_S, DATA_SHP1_AS_MOVE_U_TURN_CW_S },
                    { FSM_SIG_INPUT_MOVE_U_TURN_CW_F, DATA_SHP1_AS_MOVE_U_TURN_CW_F },
                    { FSM_SIG_INPUT_MOVE_U_TURN_CC_S, DATA_SHP1_AS_MOVE_U_TURN_CC_S },
                    { FSM_SIG_INPUT_MOVE_U_TURN_CC_F, DATA_SHP1_AS_MOVE_U_TURN_CC_F },
                    { FSM_SIG_INPUT_MOVE_D_TURN_CW_S, DATA_SHP1_AS_MOVE_D_TURN_CW_S },
                    { FSM_SIG_INPUT_MOVE_D_TURN_CW_F, DATA_SHP1_AS_MOVE_D_TURN_CW_F },
                    { FSM_SIG_INPUT_MOVE_D_TURN_CC_S, DATA_SHP1_AS_MOVE_D_TURN_CC_S },
                    { FSM_SIG_INPUT_MOVE_D_TURN_CC_F, DATA_SHP1_AS_MOVE_D_TURN_CC_F },
                    { FSM_SIG_INPUT_MOVE_C_U_S, DATA_SHP1_AS_MOVE_C_U_S },
                    { FSM_SIG_INPUT_MOVE_C_U_F, DATA_SHP1_AS_MOVE_C_U_F },
                    { FSM_SIG_INPUT_MOVE_C_UR_S, DATA_SHP1_AS_MOVE_C_UR_S },
                    { FSM_SIG_INPUT_MOVE_C_UR_F, DATA_SHP1_AS_MOVE_C_UR_F },
                    { FSM_SIG_INPUT_MOVE_C_R_S, DATA_SHP1_AS_MOVE_C_R_S },
                    { FSM_SIG_INPUT_MOVE_C_R_F, DATA_SHP1_AS_MOVE_C_R_F },
                    { FSM_SIG_INPUT_MOVE_C_DR_S, DATA_SHP1_AS_MOVE_C_DR_S },
                    { FSM_SIG_INPUT_MOVE_C_DR_F, DATA_SHP1_AS_MOVE_C_DR_F },
                    { FSM_SIG_INPUT_MOVE_C_D_S, DATA_SHP1_AS_MOVE_C_D_S },
                    { FSM_SIG_INPUT_MOVE_C_D_F, DATA_SHP1_AS_MOVE_C_D_F },
                    { FSM_SIG_INPUT_MOVE_C_DL_S, DATA_SHP1_AS_MOVE_C_DL_S },
                    { FSM_SIG_INPUT_MOVE_C_DL_F, DATA_SHP1_AS_MOVE_C_DL_F },
                    { FSM_SIG_INPUT_MOVE_C_L_S, DATA_SHP1_AS_MOVE_C_L_S },
                    { FSM_SIG_INPUT_MOVE_C_L_F, DATA_SHP1_AS_MOVE_C_L_F },
                    { FSM_SIG_INPUT_MOVE_C_UL_S, DATA_SHP1_AS_MOVE_C_UL_S },
                    { FSM_SIG_INPUT_MOVE_C_UL_F, DATA_SHP1_AS_MOVE_C_UL_F },
                    { FSM_SIG_INPUT_TURN_P_CW_S, DATA_SHP1_AS_TURN_P_CW_S },
                    { FSM_SIG_INPUT_TURN_P_CW_F, DATA_SHP1_AS_TURN_P_CW_F },
                    { FSM_SIG_INPUT_TURN_P_CC_S, DATA_SHP1_AS_TURN_P_CC_S },
                    { FSM_SIG_INPUT_TURN_P_CC_F, DATA_SHP1_AS_TURN_P_CC_F },
                    { FSM_SIG_INPUT_SHOOT, DATA_SHP1_AS_SHOOT },
                    { FSM_SIG_INPUT_SHOOT_REL, DATA_SHP1_AS_SHOOT_REL },
                    { FSM_SIG_INPUT_USE, DATA_SHP1_AS_USE },
                    { FSM_SIG_STRIKE, DATA_SHP1_AS_REEL },
                    { FSM_SIG_DIE, DATA_SHP1_AS_DEAD_REEL },
                    { FSM_SIG_ANNIHILATE, DATA_SHP1_AS_ANNIHILATE },
                    { FSM_SIG_PEACEFUL_DEATH, DATA_SHP1_AS_DEAD_REEL },
                    { -1, -1 },
                },
            },
            [DATA_SHP1_AS_REEL] = {
                .elems = (struct fsm_state_elem_t[]) {
                    { FSM_SIG_INPUT_SHOOT, DATA_SHP1_AS_SHOOT },
                    { FSM_SIG_INPUT_SHOOT_REL, DATA_SHP1_AS_SHOOT_REL },
                    { FSM_SIG_STRIKE, DATA_SHP1_AS_REEL },
                    { FSM_SIG_BUMP_LEFT, DATA_SHP1_AS_BOUNCE_H },
                    { FSM_SIG_BUMP_RIGHT, DATA_SHP1_AS_BOUNCE_H },
                    { FSM_SIG_BUMP_TOP, DATA_SHP1_AS_BOUNCE_V },
                    { FSM_SIG_BUMP_BOTTOM, DATA_SHP1_AS_BOUNCE_V },
                    { FSM_SIG_DIE, DATA_SHP1_AS_DEAD_REEL },
                    { FSM_SIG_ANNIHILATE, DATA_SHP1_AS_ANNIHILATE },
                    { FSM_SIG_ACTION_DONE, DATA_SHP1_AS_STAND },
                    { -1, -1 },
                },
            },
            [DATA_SHP1_AS_DEAD_IDLE] = {
                .elems = (struct fsm_state_elem_t[]) {
                    { FSM_SIG_STRIKE, DATA_SHP1_AS_DEAD_REEL },
                    { FSM_SIG_DIE, DATA_SHP1_AS_DEAD_REEL },
                    { FSM_SIG_BUMP_LEFT, DATA_SHP1_AS_BOUNCE_H },
                    { FSM_SIG_BUMP_RIGHT, DATA_SHP1_AS_BOUNCE_H },
                    { FSM_SIG_BUMP_TOP, DATA_SHP1_AS_BOUNCE_V },
                    { FSM_SIG_BUMP_BOTTOM, DATA_SHP1_AS_BOUNCE_V },
                    { FSM_SIG_ANNIHILATE, DATA_SHP1_AS_ANNIHILATE },
                    { -1, -1 },
                }
            },
            [DATA_SHP1_AS_DEAD_REEL] = {
                .elems = (struct fsm_state_elem_t[]) {
                    { FSM_SIG_STRIKE, DATA_SHP1_AS_DEAD_REEL },
                    { FSM_SIG_DIE, DATA_SHP1_AS_DEAD_REEL },
                    { FSM_SIG_BUMP_LEFT, DATA_SHP1_AS_BOUNCE_H },
                    { FSM_SIG_BUMP_RIGHT, DATA_SHP1_AS_BOUNCE_H },
                    { FSM_SIG_BUMP_TOP, DATA_SHP1_AS_BOUNCE_V },
                    { FSM_SIG_BUMP_BOTTOM, DATA_SHP1_AS_BOUNCE_V },
                    { FSM_SIG_ANNIHILATE, DATA_SHP1_AS_ANNIHILATE },
                    { FSM_SIG_ACTION_DONE, DATA_SHP1_AS_DEAD_IDLE },
                    { -1, -1 },
                },
            },
            [DATA_SHP1_AS_TURN_CW_S]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_TURN_CW_F]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_TURN_CC_S]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_TURN_CC_F]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_U_S]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_U_F]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_UR_S]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_UR_F]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_R_S]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_R_F]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_DR_S]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_DR_F]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_D_S]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_D_F]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_DL_S]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_DL_F]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_L_S]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_L_F]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_UL_S]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_UL_F]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_U_TURN_CW_S] = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_U_TURN_CW_F] = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_U_TURN_CC_S] = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_U_TURN_CC_F] = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_D_TURN_CW_S] = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_D_TURN_CW_F] = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_D_TURN_CC_S] = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_D_TURN_CC_F] = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_U_S]       = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_U_F]       = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_UR_S]      = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_UR_F]      = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_R_S]       = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_R_F]       = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_DR_S]      = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_DR_F]      = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_D_S]       = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_D_F]       = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_DL_S]      = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_DL_F]      = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_L_S]       = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_L_F]       = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_UL_S]      = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_MOVE_C_UL_F]      = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_TURN_P_CW_S]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_TURN_P_CW_F]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_TURN_P_CC_S]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_TURN_P_CC_F]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_BOUNCE_H]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_BOUNCE_V]         = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_SHOOT]            = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_SHOOT_REL]        = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_USE]              = { .flags = FSM_F_PSEUDO },
            [DATA_SHP1_AS_ANNIHILATE]       = { .flags = FSM_F_PSEUDO },
        },
        .num_states = DATA_SHP1_AS_MAX,
        .names = data_shp1_as_names,
    },

    .frames = af,
    .num_frames = DATA_SHP1_AF_MAX,
    .frame_names = data_shp1_af_names,
};

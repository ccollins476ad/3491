#ifndef H_DATA_
#define H_DATA_

#include "life/image.h"
#include "life/terr_defs.h"
#include "life/terr.h"


extern struct gesture_t *data_gestures;
extern struct gesture_fsm_mapping_t **data_gesture_fsm_map_basic;

extern struct terr_properties_t data_terr[TERR_ID_MAX];
extern struct image_t data_images[IMAGE_ID_CNT];

void data_gesture_init(void);

#endif

#ifndef H_DATA_
#define H_DATA_

#include "life/being.h"
#include "life/blood.h"
#include "life/image.h"
#include "life/spark.h"
#include "life/terr_defs.h"
#include "life/terr.h"
#include "life/violence.h"

extern struct gesture_t *data_gestures;
extern struct gesture_fsm_mapping_t **data_gesture_fsm_map_basic;
extern struct gun_properties_t data_gun_properties[GUN_TYPE_MAX];
extern struct blood_properties_t data_blood_properties[BLOOD_TYPE_MAX];
extern struct spark_properties_t data_spark_properties[SPARK_TYPE_MAX];

extern struct being_fsm_t data_shp1_being_fsm;

extern struct terr_properties_t data_terr[TERR_ID_MAX];
extern struct image_t data_images[IMAGE_ID_CNT];

extern struct being_properties_t data_being_properties[BEING_TYPE_MAX];

extern struct violence_seq_t data_violence_seqs[VIOLENCE_TYPE_MAX];

void data_gesture_init(void);

#endif

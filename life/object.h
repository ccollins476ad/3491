#ifndef H_OBJECT_
#define H_OBJECT_

#include <inttypes.h>
#include "gfx/icon.h"
#include "life/combat.h"
#include "life/entity.h"

#define OBJECT_TYPE_NULL                    0
#define OBJECT_TYPE_DOOR_V_CLOSED           1
#define OBJECT_TYPE_DOOR_V_OPEN             2
#define OBJECT_TYPE_DOOR_H_CLOSED           3
#define OBJECT_TYPE_DOOR_H_OPEN             4
#define OBJECT_TYPE_STAIRS_DUMMY            5
#define OBJECT_TYPE_BARREL                  6
#define OBJECT_TYPE_MAX                     7

#define OBJECT_LEVEL_SPECIFIC               10000

#define OBJECT_PROPERTIES_F_OPAQUE          0x01

#define OBJECT_EFFECT_NULL                  (-1)
#define OBJECT_EFFECT_CHANGE_TYPE           0
#define OBJECT_EFFECT_TELEPORT              1
#define OBJECT_EFFECT_BLAST                 2

#define OBJECT_K_TYPE_ON_USE                0
#define OBJECT_K_LEVEL                      1
#define OBJECT_K_X                          2
#define OBJECT_K_Y                          3
#define OBJECT_K_BLAST_PROPERTIES           4

#define OBJECT_F_TRASH                      0x01

struct object_properties_t {
    int type;
    struct icon_t icon;
    uint8_t flags;
    struct hit_box_desc_t *hit_boxes;
    struct defense_properties_t defense;

    int *effects;
    struct kv_elem_t *kv;
};

struct object_t {
    struct entity_t entity;

    struct object_properties_t *properties;
    uint8_t flags;
    int hit_points;
    int x;
    int y;
};

struct object_data_t {
    struct slab_t slab;
};

struct object_t *object_add(struct object_properties_t *properties,
                            int x, int y);
void object_clear(void);
void object_center(struct object_t *object, int *centerx, int *centery);
void object_use(struct object_t *object);
void object_die(struct object_t *object);
void object_process_hit_boxes(void);
void object_update_all(void);
void object_draw_all(void);
void object_init(void);

extern struct object_data_t object_data;

#endif


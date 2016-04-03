#ifndef HIT_BOX_H_
#define HIT_BOX_H_

#include <inttypes.h>
#include "gen/gen_shape.h"
#include "gen/container.h"
struct entity_t;

#define HIT_BOX_TYPE_POINT              0
#define HIT_BOX_TYPE_RECTANGLE          1
#define HIT_BOX_TYPE_CIRCLE             2
#define HIT_BOX_TYPE_MAX                3

#define HIT_BOX_F_CLP                   0x01
#define HIT_BOX_F_ATK                   0x02
#define HIT_BOX_F_DEF                   0x04
#define HIT_BOX_F_USE                   0x10
#define HIT_BOX_F_TCH                   0x20
#define HIT_BOX_F_SLD                   0x40

#define HIT_BOX_F_ALL                   (HIT_BOX_F_CLP | HIT_BOX_F_ATK |    \
                                         HIT_BOX_F_DEF | HIT_BOX_F_USE |    \
                                         HIT_BOX_F_TCH | HIT_BOX_F_SLD)

#define HIT_BOX_DESC_NONE               { .shape = { .type = -1 } }

struct hit_box_desc_t {
    uint8_t flags;
    struct shape_t shape;
};

struct hit_box_t {
    uint8_t flags;
    struct shape_t shape;
    struct entity_t *owner;
};

struct hit_box_intersection_t {
    struct hit_box_t *a; /* atk */
    struct hit_box_t *b; /* def */
};

extern struct pslab_t hit_box_intersections;
extern struct pslab_t hit_box_boxes;

void hit_box_clear(void);
struct hit_box_t *hit_box_add(struct hit_box_t *hit_box);
void hit_box_desc_center(struct hit_box_desc_t *desc,
                         int *centerx, int *centery);
void hit_box_from_desc(struct hit_box_t *hit_box,
                       struct hit_box_desc_t *desc,
                       int worldx, int worldy);
void hit_box_adjust_for_speed(struct hit_box_t *hit_box,
                              int speedx, int speedy);

void hit_box_draw_all(void);
void hit_box_process_atk(void);
void hit_box_process_tch(void);
int hit_box_any_in_area(struct rectangle_t *area, uint8_t hit_box_flags);

struct hit_box_t *hit_box_first_in_path(int x1, int y1, int x2, int y2,
                                        uint8_t hit_box_type);
void hit_box_set_bounds(int x, int y, int dimx, int dimy);
int hit_box_batch_is_hostile(int dmnx, int dmny, int dmnw, int dmnh,
                             int trajx, int trajy, int trajw, int trajh);
void hit_box_deinit(void);
void hit_box_init(void);

#define HIT_BOX_DESCS_FOREACH(descs, desc_cur) \
    if ((desc_cur) = NULL, (descs) != NULL) \
        for ((desc_cur) = (descs); (desc_cur)->shape.type != -1; (desc_cur)++)

#endif


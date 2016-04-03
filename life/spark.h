#ifndef H_SPARK_
#define H_SPARK_

#include "life/debris.h"

#define SPARK_TYPE_EXPLOSION    0
#define SPARK_TYPE_FIRE         1
#define SPARK_TYPE_SMOKE        2
#define SPARK_TYPE_MAX          3

struct spark_properties_t {
    struct particle_properties_t particle_properties;
    uint8_t flags;
};

struct spark_t {
    struct spark_properties_t *properties;
    struct particle_t particle;
};

struct spark_t *spark_add(struct spark_properties_t *properties,
                          int frame_duration);
void spark_update_all(void);
void spark_draw_all(void);
void spark_clear(void);
void spark_init(void);

#endif


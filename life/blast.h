#ifndef H_BLAST_
#define H_BLAST_

#include "debris.h"
struct shrapnel_properties_t;
struct proxy_attack_properties_t;

struct explosion_properties_t {
    struct spark_properties_t *smoke_spark_properties;
    int smoke_radius;
    int smoke_frame_dur_base;
    int smoke_frame_dur_rand;

    struct erupt_t fire_erupt;
    struct spark_properties_t *fire_spark_properties;
    int fire_count_base;
    int fire_count_rand;
    int fire_frame_dur_base;
    int fire_frame_dur_rand;
};

struct blast_properties_t {
    int attack_delay;
    struct explosion_properties_t *explosion_properties;
    struct shrapnel_properties_t *shrapnel_properties;
    struct proxy_attack_properties_t *proxy_properties;
};

void blast_apply(struct blast_properties_t *properties, int centerx,
                 int centery, unsigned int shrapnel_owner_id);
void blast_apply_attack(struct blast_properties_t *properties, int centerx,
                        int centery, unsigned int shrapnel_owner_id);
void blast_schedule(struct blast_properties_t *properties, int centerx,
                    int centery, unsigned int shrapnel_owner_id,
                    int ticks_from_now);
#endif

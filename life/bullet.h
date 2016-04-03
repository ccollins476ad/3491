#ifndef BULLET_H_
#define BULLET_H_

#include "life/phys.h"
#include "gen/ticker.h"
#include "gfx/icon.h"
#include "life/hit_box.h"
#include "life/combat.h"
#include "life/entity.h"
struct combat_ignore_entry_t;
struct shrapnel_properties_t;
struct spark_properties_t;
struct blast_properties_t;

#define BULLET_PROPERTIES_F_IGNORE_OWNER   0x01

struct bullet_smoke_properties_t {
    struct spark_properties_t *spark_properties;
    int frame_dur_base;
    int frame_dur_rand;
    int rate_base;
    int rate_rand;
};

struct bullet_properties_t {
    struct icon_t icon;
    struct hit_box_desc_t *hit_boxes;
    struct attack_t attack;
    struct bullet_smoke_properties_t *smoke_properties;
    struct blast_properties_t *blast_properties;
    struct attack_scale_info_t *scale_info;
    uint8_t survive_type;
    uint8_t flags;
};

struct bullet_t {
    struct entity_t entity;

    struct phys_t phys;
    struct bullet_properties_t *properties;
    uint8_t flags;
    struct combat_ignore_entry_t *ignore_list;
    struct ticker_t smoke_ticker;
    unsigned int owner_id;
    unsigned int birth_tick;
    int team_id;
};

struct shrapnel_properties_t {
    struct bullet_properties_t *bullet_properties;
    int count_base;
    int count_rand;
    struct erupt_t erupt;
    /* XXX: Angle info. */
};

void bullet_center_offset(struct bullet_properties_t *properties,
                          int *offsetx, int *offsety);
struct bullet_t *bullet_add(struct bullet_properties_t *properties);
void bullet_update_all(void);
void bullet_draw_all(void);
void bullet_clear(void);
void bullet_shrapnel_add(int centerx, int centery, unsigned int owner_id,
                         struct shrapnel_properties_t *properties);
int bullet_survives_strike(int survive_type, int was_dead, int is_dead);
void bullet_impact(struct bullet_t *bullet, int centerx, int centery);
int bullet_init(void);

#endif


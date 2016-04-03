#ifndef COMBAT_H_
#define COMBAT_H_

#include <inttypes.h>
#include "life/entity.h"
#include "life/debris.h"
#include "life/hit_box.h"
struct combat_ignore_entry_t;
struct carnage_info_t;
struct bullet_t;
struct being_t;
struct violence_seq_t;

typedef uint8_t attack_flags_t;
typedef uint8_t defense_flags_t;

#define ATTACK_F_BULLET                 0x01
#define ATTACK_F_EXPLOSION              0x02
#define ATTACK_F_ETHEREAL               0x04

#define ATTACK_MASK_ALL_TYPES           (ATTACK_F_BULLET | ATTACK_F_EXPLOSION)

#define PROXY_ATTACK_F_TRASH            0x01

#define BLAST_TYPE_NULL                 0
#define BLAST_TYPE_ROCKET               1
#define BLAST_TYPE_CLUSTER              2
#define BLAST_TYPE_NUCLEAR              3
#define BLAST_TYPE_MAX                  4

#define DEFENSE_F_ETHEREAL              0x01

#define ATTACK_SCALE_INFO_MAX_THRESHES  16

struct attack_t {
    struct violence_seq_t *violence_seq;

    int type;
    int damage;
    int reel;
    int stun;
    attack_flags_t flags;
};

struct attack_scale_thresh_t {
    int max_distance;
    double scale;
};

struct attack_scale_info_t {
    struct attack_scale_thresh_t *threshes;
    double minus_per_dist;
};

struct proxy_attack_properties_t {
    struct attack_t attack;
    struct hit_box_desc_t hit_box;
    struct attack_scale_info_t scale_info;
    int duration;
};

struct proxy_attack_t {
    struct entity_t entity;

    struct proxy_attack_properties_t *properties;
    uint8_t flags;
    int age;
    int x;
    int y;
};

struct defense_properties_t {
    int hit_points;
    attack_flags_t damaged_by;
    defense_flags_t flags;
};

int combat_attack_damage(struct attack_t *attack,
                         struct defense_properties_t *defense);
int combat_is_strike(attack_flags_t attack_flags,
                     defense_flags_t defense_flags);
int combat_bullet_vs_wall(int scapex, int scapey, struct bullet_t *bullet);
void combat_process_attack(struct hit_box_t *atk, struct hit_box_t *def);
void combat_process_hit_boxes(void);
void combat_shoot(struct being_t *being);
int combat_ignore(struct combat_ignore_entry_t **list, unsigned int id);
void combat_free_ignore_list(struct combat_ignore_entry_t *list);
struct proxy_attack_t *combat_proxy_attack_add(void);
void combat_update(void);
void combat_init(void);

#endif


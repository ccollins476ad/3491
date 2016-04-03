#ifndef GUN_H_
#define GUN_H_

#include <inttypes.h>
#include "life/debris.h"

#define GUN_TYPE_NULL                   (-1)
#define GUN_TYPE_PISTOL                 0
#define GUN_TYPE_SHOTGUN                1
#define GUN_TYPE_MACHINE_GUN            2
#define GUN_TYPE_CLOUD_KILL             3
#define GUN_TYPE_ROCKET_LAUNCHER        4
#define GUN_TYPE_CLUSTER_LAUNCHER       5
#define GUN_TYPE_BLADE                  6
#define GUN_TYPE_NUCLEAR                7
#define GUN_TYPE_MAX                    8

#define GUN_PROPERTIES_F_AUTO           0x01

#define GUN_F_TRIGGER_HELD              0x01  

struct gun_properties_t {
    char *name;
    int shoot_itvl;
    struct bullet_properties_t *bullet_properties;
    struct erupt_t erupt;
    int count_base;
    int count_rand;
    uint8_t flags;
};

struct gun_t {
    struct gun_properties_t *properties;
    uint32_t prev_shoot_time;
    uint8_t flags;
};

int gun_can_shoot(struct gun_t *gun, uint32_t now);
int gun_shoot(struct gun_t *gun, uint32_t now);
int gun_avg_speed(struct gun_properties_t *gun_properties);

#endif


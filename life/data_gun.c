#include "gfx/gfx.h"
#include "life/gun.h"
#include "life/world.h"
#include "life/life_defs.h"
#include "life/blood.h"
#include "life/blast.h"
#include "life/spark.h"
#include "life/carnage.h"
#include "life/violence.h"
#include "life/bullet_defs.h"
#include "life/bullet.h"
#include "life/data.h"

struct blast_properties_t data_blast_properties[BLAST_TYPE_MAX] = {
    [BLAST_TYPE_ROCKET] = {
        .explosion_properties = (struct explosion_properties_t[1]) { {
            .smoke_spark_properties = data_spark_properties +
                                      SPARK_TYPE_EXPLOSION,
            .smoke_radius = 6,
            .smoke_frame_dur_base = 2,
            .smoke_frame_dur_rand = 5,
            .fire_erupt = {
                .speed_base = WORLD_FACTOR / 2.5,
                .speed_rand = WORLD_FACTOR * 2,
                .angle_base = 0,
                .angle_rand = 359,
            },
            .fire_spark_properties = data_spark_properties + SPARK_TYPE_FIRE,
            .fire_count_base = 60,
            .fire_count_rand = 0,
            .fire_frame_dur_base = 4,
            .fire_frame_dur_rand = 4,
        } },
        .proxy_properties = (struct proxy_attack_properties_t[1]) { {
            .attack = {
                .violence_seq = data_violence_seqs + VIOLENCE_TYPE_ROCKET_PROX,
                .flags = ATTACK_F_EXPLOSION | ATTACK_F_ETHEREAL,
                .damage = 10000,
                .reel = 7 * WORLD_FACTOR,
                .stun = 100,
            },
            .hit_box = {
                .flags = HIT_BOX_F_ATK,
                .shape = {
                    .type = SHAPE_TYPE_CIRCLE,
                    .circle = { .radius = 6 * TILE_DIM }
                },
            },
            .scale_info = {
                .minus_per_dist = 1.0 / (10 * WORLD_TILE_DIM),
            },
            .duration = 1,
        } },
        .attack_delay = 10,
    },
    [BLAST_TYPE_CLUSTER] = {
        .explosion_properties = (struct explosion_properties_t[1]) { {
            .smoke_spark_properties = data_spark_properties +
                                      SPARK_TYPE_EXPLOSION,
            .smoke_radius = 3,
            .smoke_frame_dur_base = 2,
            .smoke_frame_dur_rand = 5,
            .fire_erupt = {
                .speed_base = WORLD_FACTOR / 1.5,
                .speed_rand = WORLD_FACTOR,
                .angle_base = 0,
                .angle_rand = 359,
            },
            .fire_spark_properties = data_spark_properties + SPARK_TYPE_FIRE,
            .fire_count_base = 50,
            .fire_count_rand = 0,
            .fire_frame_dur_base = 3,
            .fire_frame_dur_rand = 3,
        } },
        .proxy_properties = (struct proxy_attack_properties_t[1]) { {
            .attack = {
                .violence_seq = data_violence_seqs + VIOLENCE_TYPE_ROCKET_PROX,
                .flags = ATTACK_F_EXPLOSION | ATTACK_F_ETHEREAL,
                .damage = 3000,
                .reel = 7 * WORLD_FACTOR,
                .stun = 100,
            },
            .hit_box = {
                .flags = HIT_BOX_F_ATK,
                .shape = {
                    .type = SHAPE_TYPE_CIRCLE,
                    .circle = { .radius = 3 * TILE_DIM }
                },
            },
            .scale_info = {
                .minus_per_dist = 1.0 / (4 * WORLD_TILE_DIM),
            },
            .duration = 1,
        } },
    },
    [BLAST_TYPE_NUCLEAR] = {
        .explosion_properties = (struct explosion_properties_t[1]) { {
            .smoke_spark_properties = data_spark_properties +
                                      SPARK_TYPE_EXPLOSION,
            .smoke_radius = 15,
            .smoke_frame_dur_base = 2,
            .smoke_frame_dur_rand = 10,
            .fire_erupt = {
                .speed_base = WORLD_FACTOR * 1.25,
                .speed_rand = WORLD_FACTOR * 4,
                .angle_base = 0,
                .angle_rand = 359,
            },
            .fire_spark_properties = data_spark_properties + SPARK_TYPE_FIRE,
            .fire_count_base = 500,
            .fire_count_rand = 0,
            .fire_frame_dur_base = 4,
            .fire_frame_dur_rand = 4,
        } },
        .proxy_properties = (struct proxy_attack_properties_t[1]) { {
            .attack = {
                .violence_seq = data_violence_seqs + VIOLENCE_TYPE_ROCKET_PROX,
                .flags = ATTACK_F_EXPLOSION | ATTACK_F_ETHEREAL,
                .damage = 10000,
                .reel = 7 * WORLD_FACTOR,
                .stun = 100,
            },
            .hit_box = {
                .flags = HIT_BOX_F_ATK,
                .shape = {
                    .type = SHAPE_TYPE_CIRCLE,
                    .circle = { .radius = 15 * TILE_DIM }
                },
            },
            .scale_info = {
                .minus_per_dist = 1.0 / (30.0 * WORLD_TILE_DIM),
            },
            .duration = 1,
        } },
    },
};


static struct hit_box_desc_t data_bullet_hit_boxes[] = { {
    .flags = HIT_BOX_F_ATK | HIT_BOX_F_CLP,
    .shape = {
        .type = SHAPE_TYPE_RECTANGLE,
        .rectangle = { 3, 3, 1, 1, }
    },
}, HIT_BOX_DESC_NONE
};

struct bullet_properties_t data_bullet_properties[BULLET_TYPE_MAX] = {
    [BULLET_TYPE_PISTOL] = {
        .icon = { 249, COLOR(128, 128, 128)},
        .hit_boxes = data_bullet_hit_boxes,
        .attack = {
            .violence_seq = data_violence_seqs + VIOLENCE_TYPE_PISTOL,
            .flags = ATTACK_F_BULLET,
            .damage = 1100,
            .reel = 1 * WORLD_FACTOR,
            .stun = 25,
        },
        .scale_info = (struct attack_scale_info_t[1]) { {
            .threshes = (struct attack_scale_thresh_t[]) {
                /* Bullet weakens as it travels. */
                { 4  * WORLD_TILE_DIM,  1.00 },
                { 10 * WORLD_TILE_DIM,  0.75 },
                { 20 * WORLD_TILE_DIM,  0.50 },
                { INT_MAX,              0.25 },
            },
        } },
        .survive_type = BULLET_STRIKE_SURVIVE_TYPE_DEAD,
        .flags = BULLET_PROPERTIES_F_IGNORE_OWNER,
    },
    [BULLET_TYPE_BLADE] = {
        .icon = { 249, COLOR(200, 200, 20)},
        .hit_boxes = data_bullet_hit_boxes,
        .attack = {
            .violence_seq = data_violence_seqs + VIOLENCE_TYPE_BLADE,
            .flags = ATTACK_F_BULLET,
            .damage = 1000,
            .reel = 0,
            .stun = 50,
        },
        .survive_type = BULLET_STRIKE_SURVIVE_TYPE_KILL,
        .flags = BULLET_PROPERTIES_F_IGNORE_OWNER,
    },
    [BULLET_TYPE_SHOTGUN] = {
        .icon = { 249, COLOR(128, 128, 128)},
        .hit_boxes = data_bullet_hit_boxes,
        .attack = {
            .violence_seq = data_violence_seqs + VIOLENCE_TYPE_SHOTGUN,
            .flags = ATTACK_F_BULLET,
            .damage = 300,
            .reel = 1 * WORLD_FACTOR,
            .stun = 50,
        },
        .survive_type = BULLET_STRIKE_SURVIVE_TYPE_NEVER,
        .flags = BULLET_PROPERTIES_F_IGNORE_OWNER,
    },
    [BULLET_TYPE_MACHINE_GUN] = {
        .icon = { 249, COLOR(128, 128, 128)},
        .hit_boxes = data_bullet_hit_boxes,
        .attack = {
            .violence_seq = data_violence_seqs + VIOLENCE_TYPE_RIFLE,
            .flags = ATTACK_F_BULLET,
            .damage = 500,
            .reel = 1 * WORLD_FACTOR,
            .stun = 50,
        },
        .survive_type = BULLET_STRIKE_SURVIVE_TYPE_KILL,
        .flags = BULLET_PROPERTIES_F_IGNORE_OWNER,
    },
    [BULLET_TYPE_ROCKET] = {
        .icon = { 249, COLOR(128, 128, 230)},
        .hit_boxes = data_bullet_hit_boxes,
        .survive_type = BULLET_STRIKE_SURVIVE_TYPE_NEVER,
        .blast_properties = data_blast_properties + BLAST_TYPE_ROCKET,
        .smoke_properties = (struct bullet_smoke_properties_t[1]) { {
            .spark_properties = data_spark_properties + SPARK_TYPE_SMOKE,
            .frame_dur_base = 5,
            .frame_dur_rand = 3,
            .rate_base = 5,
            .rate_rand = 0,
        } },
        .flags = BULLET_PROPERTIES_F_IGNORE_OWNER,
    },
    [BULLET_TYPE_CLUSTER] = {
        .icon = { 249, COLOR(128, 230, 128)},
        .hit_boxes = data_bullet_hit_boxes,
        .survive_type = BULLET_STRIKE_SURVIVE_TYPE_NEVER,
        .blast_properties = data_blast_properties + BLAST_TYPE_CLUSTER,
        .smoke_properties = (struct bullet_smoke_properties_t[1]) { {
            .spark_properties = data_spark_properties + SPARK_TYPE_SMOKE,
            .frame_dur_base = 4,
            .frame_dur_rand = 3,
            .rate_base = 5,
            .rate_rand = 0,
        } },
        .flags = BULLET_PROPERTIES_F_IGNORE_OWNER,
    },
    [BULLET_TYPE_BOMB] = {
        .icon = { 249, COLOR(238, 128, 128)},
        .hit_boxes = data_bullet_hit_boxes,
        .survive_type = BULLET_STRIKE_SURVIVE_TYPE_NEVER,
        .blast_properties = data_blast_properties + BLAST_TYPE_NUCLEAR,
        .smoke_properties = (struct bullet_smoke_properties_t[1]) { {
            .spark_properties = data_spark_properties + SPARK_TYPE_SMOKE,
            .frame_dur_base = 6,
            .frame_dur_rand = 5,
            .rate_base = 10,
            .rate_rand = 0,
        } },
        .flags = BULLET_PROPERTIES_F_IGNORE_OWNER,
    },
};

struct gun_properties_t data_gun_properties[GUN_TYPE_MAX] = {
    [GUN_TYPE_PISTOL] = {
        .name = "pistol",
        .shoot_itvl = 30,

        .bullet_properties = data_bullet_properties + BULLET_TYPE_PISTOL,
        .erupt = {
            .speed_base = 4 * WORLD_FACTOR,
            .speed_rand = 0,
        },
        .count_base = 1,
        .count_rand = 0,
        .flags = 0,
    },
    [GUN_TYPE_SHOTGUN] = {
        .name = "shotgun",
        .shoot_itvl = 100,

        .bullet_properties = data_bullet_properties + BULLET_TYPE_SHOTGUN,
        .erupt = {
            .speed_base = 2 * WORLD_FACTOR,
            .speed_rand = WORLD_FACTOR * 0.20,
            .angle_base = -10/2,
            .angle_rand = 10,
        },
        .count_base = 10,
        .count_rand = 0,
        .flags = 0,
    },
    [GUN_TYPE_MACHINE_GUN] = {
        .name = "machine gun",
        .shoot_itvl = 7,

        .bullet_properties = data_bullet_properties + BULLET_TYPE_MACHINE_GUN,
        .erupt = {
            .speed_base = 3 * WORLD_FACTOR,
            .speed_rand = WORLD_FACTOR * 0.20,
            .angle_base = -6/2,
            .angle_rand = 6,
        },
        .count_base = 1,
        .count_rand = 0,
        .flags = GUN_PROPERTIES_F_AUTO,
    },
    [GUN_TYPE_CLOUD_KILL] = {
        .name = "cloud kill",
        .shoot_itvl = 1,

        .bullet_properties = data_bullet_properties + BULLET_TYPE_SHOTGUN,
        .erupt = {
            .speed_base = 1 * WORLD_FACTOR,
            .speed_rand = WORLD_FACTOR * 0.10,
            .angle_base = -12/2,
            .angle_rand = 12,
        },
        .count_base = 5,
        .count_rand = 0,
        .flags = GUN_PROPERTIES_F_AUTO,
    },
    [GUN_TYPE_ROCKET_LAUNCHER] = {
        .name = "rocket launcher",
        .shoot_itvl = 200,

        .bullet_properties = data_bullet_properties + BULLET_TYPE_ROCKET,
        .erupt = {
            .speed_base = 3 * WORLD_FACTOR,
            .speed_rand = 0,
        },
        .count_base = 1,
        .count_rand = 0,
        .flags = 0,
    },
    [GUN_TYPE_CLUSTER_LAUNCHER] = {
        .name = "cluster bomb",
        .shoot_itvl = 200,

        .bullet_properties = data_bullet_properties + BULLET_TYPE_CLUSTER,
        .erupt = {
            .speed_base = 2 * WORLD_FACTOR,
            .speed_rand = WORLD_FACTOR * 1.75,
            .angle_base = -12/2,
            .angle_rand = 12,
        },
        .count_base = 10,
        .count_rand = 0,
        .flags = 0,
    },
    [GUN_TYPE_BLADE] = {
        .name = "neutron canon",
        .shoot_itvl = 7,

        .bullet_properties = data_bullet_properties + BULLET_TYPE_BLADE,
        .erupt = {
            .speed_base = 3 * WORLD_FACTOR,
            .speed_rand = WORLD_FACTOR * 0.20,
            .angle_base = -6/2,
            .angle_rand = 6,
        },
        .count_base = 1,
        .count_rand = 0,
        .flags = GUN_PROPERTIES_F_AUTO,
    },
    [GUN_TYPE_NUCLEAR] = {
        .name = "nuclear",
        .shoot_itvl = 200,

        .bullet_properties = data_bullet_properties + BULLET_TYPE_BOMB,
        .erupt = {
            .speed_base = 1.5 * WORLD_FACTOR,
            .speed_rand = 0,
        },
        .count_base = 1,
        .count_rand = 0,
        .flags = 0,
    },
};

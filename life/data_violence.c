#include <math.h>
#include "life/carnage.h"
#include "life/data.h"
#include "life/fsm.h"
#include "life/violence.h"
#include "life/world.h"

static struct spray_t data_violence_pistol_sprays[] = { {
    .erupt = {
        .speed_base = -1.25 * WORLD_FACTOR,
        .speed_rand = 1 * WORLD_FACTOR,
        .angle_base = -45/2,
        .angle_rand = 45,
    },
    .count_base = 4,
    .count_rand = 4,
    .blood_type = BLOOD_TYPE_DROP,
    .frame_dur_base = 5,
    .frame_dur_rand = 5,
}, {
    .erupt = {
        .speed_base = 0.3 * WORLD_FACTOR,
        .speed_rand = 0.15 * WORLD_FACTOR,
        .angle_base = -45/2,
        .angle_rand = 45,
    },
    .count_base = 1,
    .count_rand = 0,
    .blood_type = BLOOD_TYPE_DROP,
    .frame_dur_base = 8,
    .frame_dur_rand = 8,
}, {
    .blood_type = -1,
} };

static struct spray_t data_violence_shotgun_sprays[] = { {
    .erupt = {
        .speed_base = -0.6 * WORLD_FACTOR,
        .speed_rand = 0.3 * WORLD_FACTOR,
        .angle_base = -45/2,
        .angle_rand = 45,
    },
    .count_base = 3,
    .count_rand = 5,
    .blood_type = BLOOD_TYPE_DROP,
    .frame_dur_base = 8,
    .frame_dur_rand = 8,
}, {
    .erupt = {
        .speed_base = 0.3 * WORLD_FACTOR,
        .speed_rand = 0.15 * WORLD_FACTOR,
        .angle_base = -45/2,
        .angle_rand = 45,
    },
    .count_base = 1,
    .count_rand = 0,
    .blood_type = BLOOD_TYPE_DROP,
    .frame_dur_base = 8,
    .frame_dur_rand = 8,
}, {
    .blood_type = -1,
} };


static struct spray_t data_violence_hunting_rifle_sprays[] = { {
    .erupt = {
        .speed_base = 0.3 * WORLD_FACTOR,
        .speed_rand = 0.3 * WORLD_FACTOR,
        .angle_base = 180 - 90/2 + 180,
        .angle_rand = 90,
    },
    .count_base = 3,
    .count_rand = 4,
    .blood_type = BLOOD_TYPE_DROP,
    .frame_dur_base = 5,
    .frame_dur_rand = 5,
}, {
    .erupt = {
        .speed_base = 0.3 * WORLD_FACTOR,
        .speed_rand = 1.0 * WORLD_FACTOR,
        .angle_base = -10/2 + 180,
        .angle_rand = 10,
    },
    .count_base = 8,
    .count_rand = 8,
    .blood_type = BLOOD_TYPE_DROP,
    .frame_dur_base = 4,
    .frame_dur_rand = 8,
}, {
    .blood_type = -1,
} };

struct violence_seq_t data_violence_seqs[VIOLENCE_TYPE_MAX] = {
    [VIOLENCE_TYPE_PISTOL] = {
        .entries = (struct violence_t[]) {
            [0] = {
                .signal = FSM_SIG_DIE,
                .min_damage = 0,
                .max_hp = 0,
                .chance = 100,
                .sprays = data_violence_pistol_sprays,
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },

            [1] = {
                .signal = FSM_SIG_STRIKE,
                .min_damage = 0,
                .max_hp = INT_MAX,
                .chance = 100,
                .sprays = data_violence_pistol_sprays,
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },
        },
    },

    [VIOLENCE_TYPE_RIFLE] = {
        .entries = (struct violence_t[]) {
            [0] = {
                .signal = FSM_SIG_DIE,
                .min_damage = 0,
                .max_hp = 0,
                .chance = 100,
                .sprays = (struct spray_t[]) { {
                    .erupt = {
                        .speed_base = -0.6 * WORLD_FACTOR,
                        .speed_rand = 0.3 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 3,
                    .count_rand = 5,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .erupt = {
                        .speed_base = 0.3 * WORLD_FACTOR,
                        .speed_rand = 0.15 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 1,
                    .count_rand = 0,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .blood_type = -1,
                } },
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },

            [1] = {
                .signal = FSM_SIG_STRIKE,
                .min_damage = 0,
                .max_hp = INT_MAX,
                .chance = 100,
                .sprays = (struct spray_t[]) { {
                    .erupt = {
                        .speed_base = -0.6 * WORLD_FACTOR,
                        .speed_rand = 0.3 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 3,
                    .count_rand = 5,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .erupt = {
                        .speed_base = 0.3 * WORLD_FACTOR,
                        .speed_rand = 0.15 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 1,
                    .count_rand = 0,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .blood_type = -1,
                } },
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },
        },
    },

    [VIOLENCE_TYPE_SHOTGUN] = {
        .entries = (struct violence_t[]) {
            [0] = {
                .signal = FSM_SIG_DIE,
                .min_damage = 0,
                .max_hp = 0,
                .chance = 100,
                .sprays = data_violence_shotgun_sprays,
                .bleed_chance = 50,
                .bleed_dec = 1,
                .carnage_info = NULL,
                .star_info = NULL,
            },

            [1] = {
                .signal = FSM_SIG_STRIKE,
                .min_damage = 0,
                .max_hp = INT_MAX,
                .chance = 100,
                .sprays = data_violence_shotgun_sprays,
                .bleed_chance = 50,
                .bleed_dec = 1,
                .carnage_info = NULL,
                .star_info = NULL,
            },
        },
    },

    [VIOLENCE_TYPE_HUNTING_RIFLE] = {
        .entries = (struct violence_t[]) {
            [0] = {
                .signal = FSM_SIG_DIE,
                .min_damage = 0,
                .max_hp = 0,
                .chance = 100,
                .sprays = data_violence_hunting_rifle_sprays,
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },

            [1] = {
                .signal = FSM_SIG_STRIKE,
                .min_damage = 0,
                .max_hp = INT_MAX,
                .chance = 100,
                .sprays = data_violence_hunting_rifle_sprays,
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },
        },
    },

    [VIOLENCE_TYPE_ROCKET_PROX] = {
        .entries = (struct violence_t[]) {
            [0] = {
                .signal = FSM_SIG_ANNIHILATE,
                .min_damage = 1500,
                .max_hp = 0,
                .chance = 25,
                .sprays = (struct spray_t[]) { {
                    .erupt = {
                        .speed_base = 0.50 * WORLD_FACTOR,
                        .speed_rand = 0.50 * WORLD_FACTOR,
                        .angle_base = -120,
                        .angle_rand = 240,
                    },
                    .count_base = 17,
                    .count_rand = 20,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 10,
                    .frame_dur_rand = 10,
                }, {
                    .blood_type = -1,
                } },
                .bleed_chance = 25,
                .bleed_dec = 1,

                .carnage_info = (struct carnage_info_t[1]) { {
                    .type = CARNAGE_TYPE_SLICE,
                    .rads_off_base = -0.25 * M_PI,
                    .rads_off_rand = 0.5 * M_PI,
                    .speed_base = 25 * WORLD_FACTOR,
                    .speed_rand = 25 * WORLD_FACTOR,
                    .rotate_speed_base = -1000 * WORLD_FACTOR,
                    .rotate_speed_rand = 2000 * WORLD_FACTOR,
                } },
                .star_info = (struct stain_star_info_t[1]) { {
                    .length_base = 0,
                    .length_rand = 4,
                    .angle_base = 0,
                    .angle_rand = 359,
                    .count_base = 1,
                    .count_rand = 3,
                    .frame_dur_base = 256 * 100 / 25,
                    .frame_dur_rand = 256 * 100 / 50,
                } },
            },
            [1] = {
                .signal = FSM_SIG_ANNIHILATE,
                .min_damage = 1500,
                .max_hp = 0,
                .chance = 100,
                .sprays = (struct spray_t[]) { {
                    .erupt = {
                        .speed_base = 0.50 * WORLD_FACTOR,
                        .speed_rand = 0.50 * WORLD_FACTOR,
                        .angle_base = -120,
                        .angle_rand = 240,
                    },
                    .count_base = 17,
                    .count_rand = 20,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 10,
                    .frame_dur_rand = 10,
                }, {
                    .blood_type = -1,
                } },
                .bleed_chance = 25,
                .bleed_dec = 1,

                .carnage_info = (struct carnage_info_t[1]) { {
                    .type = CARNAGE_TYPE_BITS,
                    .rads_off_base = -0.25 * M_PI,
                    .rads_off_rand = 0.5 * M_PI,
                    .speed_base = 25 * WORLD_FACTOR,
                    .speed_rand = 25 * WORLD_FACTOR,
                    .rotate_speed_base = -1000 * WORLD_FACTOR,
                    .rotate_speed_rand = 2000 * WORLD_FACTOR,
                } },
                .star_info = (struct stain_star_info_t[1]) { {
                    .length_base = 0,
                    .length_rand = 4,
                    .angle_base = 0,
                    .angle_rand = 359,
                    .count_base = 1,
                    .count_rand = 3,
                    .frame_dur_base = 256 * 100 / 25,
                    .frame_dur_rand = 256 * 100 / 50,
                } },
            },
            [2] = {
                .signal = FSM_SIG_DIE,
                .min_damage = 0,
                .max_hp = 0,
                .chance = 100,
                .sprays = (struct spray_t[]) { {
                    .erupt = {
                        .speed_base = 0.50 * WORLD_FACTOR,
                        .speed_rand = 0.50 * WORLD_FACTOR,
                        .angle_base = -120,
                        .angle_rand = 240,
                    },
                    .count_base = 17,
                    .count_rand = 20,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 10,
                    .frame_dur_rand = 10,
                }, {
                    .blood_type = -1,
                } },
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },

            [3] = {
                .signal = FSM_SIG_STRIKE,
                .min_damage = 0,
                .max_hp = INT_MAX,
                .chance = 100,
                .sprays = (struct spray_t[]) { {
                    .erupt = {
                        .speed_base = -0.6 * WORLD_FACTOR,
                        .speed_rand = 0.3 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 10,
                    .count_rand = 5,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .erupt = {
                        .speed_base = 0.3 * WORLD_FACTOR,
                        .speed_rand = 0.15 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 5,
                    .count_rand = 5,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .blood_type = -1,
                } },
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },
        },
    },

    [VIOLENCE_TYPE_BLADE] = {
        .entries = (struct violence_t[]) {
            [0] = {
                .signal = FSM_SIG_ANNIHILATE,
                .min_damage = 0,
                .max_hp = 0,
                .chance = 100,
                .sprays = (struct spray_t[]) { {
                    .erupt = {
                        .speed_base = -0.8 * WORLD_FACTOR,
                        .speed_rand = 0.5 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 6,
                    .count_rand = 8,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 10,
                    .frame_dur_rand = 10,
                }, {
                    .erupt = {
                        .speed_base = 0.3 * WORLD_FACTOR,
                        .speed_rand = 0.15 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 5,
                    .count_rand = 2,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .blood_type = -1,
                } },
                .bleed_chance = 25,
                .bleed_dec = 1,

                .carnage_info = (struct carnage_info_t[1]) { {
                    .type = CARNAGE_TYPE_SLICE,
                    .rads_off_base = -0.083 * M_PI,
                    .rads_off_rand = 0.167 * M_PI,
                    .speed_base = 10 * WORLD_FACTOR,
                    .speed_rand = 22 * WORLD_FACTOR,
                    .rotate_speed_base = -250 * WORLD_FACTOR,
                    .rotate_speed_rand = 500 * WORLD_FACTOR,
                } },
                .star_info = (struct stain_star_info_t[1]) { {
                    .length_base = 0,
                    .length_rand = 2,
                    .angle_base = 0,
                    .angle_rand = 359,
                    .count_base = 1,
                    .count_rand = 1,
                    .frame_dur_base = 256 * 100 / 25,
                    .frame_dur_rand = 256 * 100 / 50,
                } },
            },

            [1] = {
                .signal = FSM_SIG_DIE,
                .min_damage = 0,
                .max_hp = 0,
                .chance = 100,
                .sprays = (struct spray_t[]) { {
                    .erupt = {
                        .speed_base = -0.6 * WORLD_FACTOR,
                        .speed_rand = 0.3 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 3,
                    .count_rand = 5,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .erupt = {
                        .speed_base = 0.3 * WORLD_FACTOR,
                        .speed_rand = 0.15 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 1,
                    .count_rand = 0,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .blood_type = -1,
                } },
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },

            [2] = {
                .signal = FSM_SIG_STRIKE,
                .min_damage = 0,
                .max_hp = INT_MAX,
                .chance = 100,
                .sprays = (struct spray_t[]) { {
                    .erupt = {
                        .speed_base = -0.6 * WORLD_FACTOR,
                        .speed_rand = 0.3 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 3,
                    .count_rand = 5,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .erupt = {
                        .speed_base = 0.3 * WORLD_FACTOR,
                        .speed_rand = 0.15 * WORLD_FACTOR,
                        .angle_base = -45/2,
                        .angle_rand = 45,
                    },
                    .count_base = 1,
                    .count_rand = 0,
                    .blood_type = BLOOD_TYPE_DROP,
                    .frame_dur_base = 8,
                    .frame_dur_rand = 8,
                }, {
                    .blood_type = -1,
                } },
                .bleed_chance = 100,
                .bleed_dec = 100,
                .carnage_info = NULL,
                .star_info = NULL,
            },
        },
    },
};


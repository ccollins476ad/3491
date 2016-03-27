#ifndef H_STATS_
#define H_STATS_

struct stats_terr_t {
    unsigned live_oom;
};

struct stats_being_t {
    unsigned traits_oom;
};

struct stats_bullet_t {
    unsigned oom;
};

struct stats_hit_box_t {
    unsigned oom;
    unsigned oom_atk;
    unsigned oom_def;
    unsigned oom_clp;
    unsigned oom_use;
    unsigned oom_tch;
    unsigned oom_sld;
};

struct stats_data_t {
    struct stats_terr_t terr;
    struct stats_being_t being;
    struct stats_bullet_t bullet;
    struct stats_hit_box_t hit_box;
};

extern struct stats_data_t stats_data;

#endif


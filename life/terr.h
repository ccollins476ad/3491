#ifndef TERR_H_
#define TERR_H_

#include <stdint.h>
#include "gfx/tile.h"
#include "gfx/icon.h"
struct canvas_t;

typedef uint8_t terr_id_t;

struct terr_properties_t {
    struct icon_t icon;
    uint8_t flags;

    /* Live properties: */
    terr_id_t prev_terr_id;
    terr_id_t next_terr_id;
    terr_id_t use_terr_id;
    //struct defense_properties_t defense;
    int regen_rate;
};

struct terr_live_t {
    struct terr_properties_t *properties;
    int scapex;
    int scapey;
    int hit_points;
};

struct terr_key_t {
    char c;
    int terr_id;
};

struct terr_scape_t {
    terr_id_t *terrs;
    terr_id_t default_terr;
    int width;
    int height;
};

void terr_scape_create(int width, int height, tile_id_t default_tile);
void terr_scape_destroy(void);
terr_id_t terr_scape_get_raw(int x, int y);
tile_id_t terr_scape_get(int x, int y);
void terr_scape_set(int x, int y, tile_id_t tile_id);
void terr_scape_set_opaque(int x, int y, int opaque);
int terr_nvm_load_scape(struct terr_key_t *key, int index_sz,
                        char **tiles, int tiles_ydim);

void terr_update(void);
struct terr_live_t *terr_live_find(int x, int y);
void terr_live_clear(void);
//int terr_apply_attack(int x, int y, struct attack_t *attack);
void terr_draw_scape(void);

int terr_init(void);

extern struct terr_scape_t terr_scape;

#endif


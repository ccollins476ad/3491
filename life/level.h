#ifndef H_LEVEL_
#define H_LEVEL_

#include "life/terr.h"
struct object_properties_t;

struct level_tile_t {
    char key;
    terr_id_t terr_id;
};

struct level_t {
    struct level_tile_t *tiles;
    char **rows;
};

int level_load(struct level_t *level);
extern struct level_t *level_cur;

#endif

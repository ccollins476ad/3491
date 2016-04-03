#include <stddef.h>
#include "gen/gen_num.h"
#include "gfx/gfx.h"
#include "life/world.h"
#include "life/spark.h"
#include "life/bullet.h"
#include "life/event.h"
#include "life/blast.h"

struct explosion_arg_t {
    int x;
    int y;
    struct explosion_properties_t *properties;
};

static int
explosion_cb(int x, int y, void *arg)
{
    struct explosion_properties_t *properties;
    struct explosion_arg_t *cb_arg;
    struct spark_t *spark;
    int frame_dur;
    int tilex;
    int tiley;

    cb_arg = arg;
    properties = cb_arg->properties;

    frame_dur = properties->smoke_frame_dur_base +
                rand_up_to(properties->smoke_frame_dur_rand);
    spark = spark_add(properties->smoke_spark_properties, frame_dur);
    if (spark == NULL) {
        return -1;
    }

    tilex = cb_arg->x / WORLD_TILE_DIM + x;
    tiley = cb_arg->y / WORLD_TILE_DIM + y;
    spark->particle.phys.x = cb_arg->x + x * WORLD_TILE_DIM;
    spark->particle.phys.y = cb_arg->y + y * WORLD_TILE_DIM;

    return 0;
}

static void
explosion_add(int x, int y, struct explosion_properties_t *properties)
{
    struct explosion_arg_t arg;
    struct spark_t *spark;
    int fire_count;
    int frame_dur;
    int i;

    arg.x = x;
    arg.y = y;
    arg.properties = properties;

    circlefill_foreach(0, 0, properties->smoke_radius, explosion_cb, &arg);

    fire_count = properties->fire_count_base +
                 rand_up_to(properties->fire_count_rand);
    for (i = 0; i < fire_count; i++) {
        frame_dur = properties->fire_frame_dur_base +
                    rand_up_to(properties->fire_frame_dur_rand);
        spark = spark_add(properties->fire_spark_properties, frame_dur);
        if (spark == NULL) {
            break;
        }

        erupt_apply(&properties->fire_erupt, &spark->particle.phys);
        spark->particle.phys.x = x;
        spark->particle.phys.y = y;
    }
}

void
blast_apply_attack(struct blast_properties_t *properties, int centerx,
                   int centery, unsigned int shrapnel_owner_id)
{
    struct proxy_attack_t *prox;

    if (properties->proxy_properties != NULL) {
        prox = combat_proxy_attack_add();
        if (prox != NULL) {
            prox->properties = properties->proxy_properties;
            prox->x = centerx;
            prox->y = centery;
        }
    }
    if (properties->shrapnel_properties != NULL) {
        bullet_shrapnel_add(centerx, centery, shrapnel_owner_id,
                            properties->shrapnel_properties);
    }
}

void
blast_apply(struct blast_properties_t *properties, int centerx, int centery,
            unsigned int shrapnel_owner_id)
{
    struct event_t *event;

    if (properties->explosion_properties != NULL) {
        explosion_add(centerx, centery, properties->explosion_properties);
    }
    if (properties->proxy_properties != NULL ||
        properties->shrapnel_properties != NULL) {

        event = event_add(EVENT_TYPE_BLAST,
                          world_tick + properties->attack_delay);
        if (event == NULL) {
            /* Out of memory; apply blast immediately. */
            blast_apply_attack(properties, centerx, centery,
                               shrapnel_owner_id);
        } else {
            event->blast.properties = properties;
            event->blast.shrapnel_owner_id = shrapnel_owner_id;
            event->blast.x = centerx;
            event->blast.y = centery;
        }
    }
}

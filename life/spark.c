#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "gen/container.h"
#include "life/data.h"
#include "life/spark.h"

#define SPARK_MAX   (10 * 1024)

static struct {
    struct slab_t slab;
} spark_data;

struct spark_t *
spark_add(struct spark_properties_t *properties, int frame_duration)
{
    struct spark_t *spark;

    assert(frame_duration > 0);

    spark = slab_insert(&spark_data.slab);
    if (spark != NULL) {
        memset(spark, 0, sizeof *spark);
        spark->properties = properties;
        animation_player_start(
            &spark->particle.animation_player,
            properties->particle_properties.animation,
            TILE_ID_NONE,
            frame_duration);
    }

    return spark;
}

void 
spark_update_all(void)
{
    struct spark_t *spark;
    int trash;

    SLAB_FOREACH(&spark_data.slab, spark) {
        trash = particle_update(&spark->particle);
        if (trash) {
            slab_erase_elem(&spark_data.slab, spark);
        }
    }
}

void
spark_draw_all(void)
{
    struct spark_t *spark;

    SLAB_FOREACH(&spark_data.slab, spark) {
        particle_draw(&spark->particle);
    }
}

void
spark_clear(void)
{
    slab_clear(&spark_data.slab);
}

void
spark_init(void)
{
    int rc;

    slab_destroy(&spark_data.slab);

    rc = slab_create(&spark_data.slab, sizeof (struct spark_t), SPARK_MAX);
    assert(rc == 0);
}


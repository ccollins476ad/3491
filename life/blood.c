#include <assert.h>
#include "gen/gen_num.h"
#include "gen/container.h"
#include "gfx/balleg.h"
#include "gfx/gfx.h"
#include "gfx/canvas.h"
#include "life/data.h"
#include "life/world.h"
#include "life/blood.h"

#define BLOOD_MAX   (1 * 1024)
#define STAIN_MAX   (1 * 1024)

static struct {
    struct slab_t slab;
    struct pslab_t stain_pslab;
} blood_data;

static struct stain_t *
stain_find(int x, int y)
{
    struct stain_t *stain;
    int i;

    for (i = 0; i < blood_data.stain_pslab.size; i++) {
        stain = pslab_at(&blood_data.stain_pslab, i);
        if (stain->x == x && stain->y == y) {
            return stain;
        }
    }

    return NULL;
}

struct blood_t *
blood_add(int type, int frame_duration, int x, int y)
{
    struct blood_t *blood;

    assert(type >= 0 && type < BLOOD_TYPE_MAX);
    assert(frame_duration > 0);

    blood = slab_insert(&blood_data.slab);
    if (blood != NULL) {
        memset(blood, 0, sizeof *blood);
        blood->properties = data_blood_properties + type;
        blood->particle.phys.x = x;
        blood->particle.phys.y = y;
        animation_player_start(
            &blood->particle.animation_player,
            data_blood_properties[type].particle_properties.animation,
            TILE_ID_NONE,
            frame_duration);
    }

    return blood;
}

struct stain_t *
blood_stain_add(int x, int y, int frame_duration)
{
    struct stain_t *stain;

    /* Stains must be tiled. */
    world_view_loc_snap_to_tile(x, y, &x, &y);

    stain = stain_find(x, y);
    if (stain != NULL) {
        frame_duration = max(frame_duration, stain->ticker.rate);
        pslab_erase_elem(&blood_data.stain_pslab, stain);
    }

    stain = pslab_insert(&blood_data.stain_pslab);
    if (stain != NULL) {
        stain->x = x;
        stain->y = y;
        stain->alpha = 255;
        stain->ticker.rate = frame_duration;
        stain->ticker.cur = 0;
    }

    return stain;
}

static int
blood_stain_update(struct stain_t *stain)
{
    int change;

    change = ticker_update(&stain->ticker, BLOOD_STAIN_TICK);
    stain->alpha -= change;
    if (stain->alpha <= 0) {
        return 1;
    }

    return 0;
}

void
blood_update_all(void)
{
    struct stain_t *stain;
    struct blood_t *blood;
    int trash;
    int i;

    /* Update drops. */
    SLAB_FOREACH(&blood_data.slab, blood) {
        trash = particle_update(&blood->particle);
        if (trash) {
            slab_erase_elem(&blood_data.slab, blood);
        }
    }

    /* Update stains. */
    for (i = 0; i < blood_data.stain_pslab.size; /* i incremented in body */) {
        stain = pslab_at(&blood_data.stain_pslab, i);
        trash = blood_stain_update(stain);
        if (trash) {
            pslab_erase_at(&blood_data.stain_pslab, i);
        } else {
            i++;
        }
    }
}

void
blood_draw_all_top(void)
{
    struct blood_t *blood;

    SLAB_FOREACH(&blood_data.slab, blood) {
        particle_draw(&blood->particle);
    }
}

void
blood_draw_all_bottom(void)
{
    struct stain_t *stain;
    struct icon_t icon;
    int i;

    for (i = 0; i < blood_data.stain_pslab.size; i++) {
        stain = pslab_at(&blood_data.stain_pslab, i);

        icon.tile_id = TILE_ID_BLOCK_100;
        icon.fg = REDA(200, stain->alpha);
        world_view_draw_icon(&icon, NULL, stain->x, stain->y);
    }
}

void
blood_clear(void)
{
    slab_clear(&blood_data.slab);
    blood_data.stain_pslab.size = 0;
}

void
blood_init(void)
{
    int rc;

    slab_destroy(&blood_data.slab);
    pslab_destroy(&blood_data.stain_pslab);

    rc = slab_create(&blood_data.slab, sizeof (struct blood_t), BLOOD_MAX);
    assert (rc == 0);

    rc = pslab_create(&blood_data.stain_pslab, sizeof (struct stain_t),
                      STAIN_MAX);
    assert (rc == 0);
}


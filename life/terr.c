#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include "gen/container.h"
#include "gen/gen_dbg.h"
#include "gfx/balleg.h"
#include "gfx/canvas.h"
#include "life/data.h"
#include "life/world.h"
#include "life/stats.h"
#include "life/terr_defs.h"
#include "life/terr.h"

#define TERR_LIVE_MAX           (20 * 1024)

#define TERR_SCAPE_MAX_DIMX     2048
#define TERR_SCAPE_MAX_DIMY     2048

/*****************************************************************************
 * $scape                                                                    *
 *****************************************************************************/

struct terr_scape_t terr_scape;

struct {
    struct pslab_t live_pslab;
} terr_data;

void
terr_scape_create(int width, int height, terr_id_t default_terr)
{
    int x;
    int y;

    terr_scape_destroy();

    terr_scape.terrs = malloc(width*height * sizeof *terr_scape.terrs);
    assert(terr_scape.terrs != NULL);
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            terr_scape.terrs[y * width + x] = default_terr;
        }
    }

    terr_scape.width = width;
    terr_scape.height = height;
    terr_scape.default_terr = default_terr;
}

void
terr_scape_destroy(void)
{
    free(terr_scape.terrs);
    terr_scape.terrs = NULL;
}

static terr_id_t *
terr_scape_row(int y)
{
    if (y < 0 || y >= terr_scape.height) {
        return NULL;
    }
    return terr_scape.terrs + y * terr_scape.width;
}

terr_id_t
terr_scape_get_raw(int x, int y)
{
    terr_id_t *row;

    row = terr_scape_row(y);
    if (row == NULL || x < 0 || x >= terr_scape.width) {
        return terr_scape.default_terr;
    }
    return row[x];
}

terr_id_t
terr_scape_get(int x, int y)
{
    terr_id_t terr_id;

    terr_id = terr_scape_get_raw(x, y);
    return terr_id & ~TERR_ID_F_OPAQUE;
}

static void
terr_scape_set_raw(int x, int y, terr_id_t terr_id)
{
    terr_id_t *row;

    row = terr_scape_row(y);
    if (row == NULL || x < 0 || x >= terr_scape.width) {
        return;
    }

    row[x] = terr_id;
}

void
terr_scape_set(int x, int y, terr_id_t terr_id)
{
    assert(!(terr_id & TERR_ID_F_OPAQUE));

    if (data_terr[terr_id].flags & TERR_F_OPAQUE) {
        terr_id |= TERR_ID_F_OPAQUE;
    }
    terr_scape_set_raw(x, y, terr_id);
}

void
terr_scape_set_opaque(int x, int y, int opaque)
{
    terr_id_t terr_id;

    terr_id = terr_scape_get(x, y);
    if (opaque || data_terr[terr_id].flags & TERR_F_OPAQUE) {
        terr_id |= TERR_ID_F_OPAQUE;
    }

    terr_scape_set_raw(x, y, terr_id);
}

#if 0
/*****************************************************************************
 * $live                                                                     *
 *****************************************************************************/

/**
 * $return                      0 if terr is still alive.
 *                              1 if terr is dead.
 */
static int
terr_live_adjust_hp(struct terr_live_t *live, int hp_delta, int *out_absorbed)
{
    struct terr_properties_t *properties;
    terr_id_t terr_id;
    int absorbed;
    int tnt_hp;
    int rc;

    absorbed = 0;
    properties = live->properties;

    while (1) {
        tnt_hp = live->hit_points + hp_delta;
        if (tnt_hp > properties->defense.hit_points) {
            absorbed += properties->defense.hit_points - live->hit_points;
            terr_id = properties->prev_terr_id;
        } else if (tnt_hp <= 0) {
            absorbed -= live->hit_points;
            terr_id = properties->next_terr_id;
        } else {
            /* No more transitions. */
            live->hit_points = tnt_hp;
            absorbed += hp_delta;

            rc = 0;
            goto done;
        }

        /* Apply transition. */
        terr_scape_set(live->scapex, live->scapey, terr_id);
        properties = data_terr + terr_id;
        if (!(properties->flags & TERR_F_LIVE)) {
            /* Live turned into a dormant terr. */
            rc = 1;
            goto done;
        }

        live->properties = properties;
        hp_delta -= absorbed;

        if (tnt_hp <= 0) {
            live->hit_points = properties->defense.hit_points;
        } else {
            live->hit_points = 0;
        }
    }

done:
    if (out_absorbed != NULL) {
        *out_absorbed = absorbed;
    }
    return rc;
}

void
terr_update(void)
{
    struct terr_live_t *live;
    int dead;
    int i;

    for (i = 0; i < terr_data.live_pslab.size; /* i incremented in body */) {
        live = pslab_at(&terr_data.live_pslab, i);
        dead = terr_live_adjust_hp(live, live->properties->regen_rate, NULL);
        if (dead) {
            pslab_erase_elem(&terr_data.live_pslab, live);
        } else {
            i++;
        }
    }
}

struct terr_live_t *
terr_live_find(int x, int y)
{
    struct terr_live_t *live;
    int i;

    for (i = 0; i < terr_data.live_pslab.size; i++) {
        live = pslab_at(&terr_data.live_pslab, i);
        if (live->scapex == x && live->scapey == y) {
            return live;
        }
    }

    return NULL;
}

static struct terr_live_t *
terr_live_new(int x, int y, terr_id_t terr_id)
{
    struct terr_properties_t *properties;
    struct terr_live_t *live;

    properties = data_terr + terr_id;
    assert(properties->flags & TERR_F_LIVE);

    live = pslab_insert(&terr_data.live_pslab);
    if (live == NULL) {
        stats_data.terr.live_oom++;
        return NULL;
    }
    live->properties = properties;
    live->scapex = x;
    live->scapey = y;
    live->hit_points = properties->defense.hit_points;

    return live;
}

void
terr_live_clear(void)
{
    terr_data.live_pslab.size = 0;
}

/**
 * Returns the amount of damage absorbed.
 */
int
terr_apply_attack(int x, int y, struct attack_t *attack)
{
    struct terr_properties_t *properties;
    struct terr_live_t *live;
    terr_id_t terr_id;
    int tmp_absorbed;
    int hit_points;
    int absorbed;
    int damage;
    int dead;

    if (attack->damage <= 0) {
        return 0;
    }

    terr_id = terr_scape_get(x, y);
    properties = data_terr + terr_id;

    if (!(combat_is_strike(attack->flags, properties->defense.flags))) {
        return 0;
    } 

    absorbed = 0;

    if (properties->defense.hit_points == 0) {
        /* Absorb the full attack. */
        return attack->damage;
    }

    if (properties->flags & TERR_F_LIVE) {
        live = terr_live_find(x, y);
    } else {
        live = NULL;
    }

    if (live != NULL) {
        hit_points = live->hit_points;
    } else {
        hit_points = properties->defense.hit_points;
    }

    damage = combat_attack_damage(attack, &properties->defense);
    absorbed += attack->damage - damage;

    if (properties->flags & TERR_F_LIVE && live == NULL) {
        live = terr_live_new(x, y, terr_id);
    }

    if (live == NULL) {
        if (damage < hit_points) {
            absorbed += damage;
        } else {
            absorbed += hit_points;
            terr_id = properties->next_terr_id;
            terr_scape_set(x, y, terr_id);

            properties = data_terr + terr_id;
            if (properties->flags & TERR_F_LIVE) {
                live = terr_live_new(x, y, terr_id);
            }
        }

        damage -= absorbed;
    }

    if (live != NULL) {
        dead = terr_live_adjust_hp(live, -damage, &tmp_absorbed);
        absorbed -= tmp_absorbed;
        if (dead) {
            pslab_erase_elem(&terr_data.live_pslab, live);
        }
    }

    return absorbed;
}
#endif

/*****************************************************************************
 * $draw                                                                     *
 *****************************************************************************/

static void
terr_draw(struct canvas_t *canvas, int x, int y, terr_id_t terr_id)
{
    struct terr_properties_t *terr;
    struct icon_t *icon;

    terr = data_terr + terr_id;
    icon = &terr->icon;

    tile_draw_no_alpha(canvas, x, y, icon->tile_id, icon->color);
}

void
terr_draw_scape(void)
{
    struct calc_chunks_rsp_t chunk_rsp;
    terr_id_t terr_id;
    int canvasx;
    int canvasy;
    int x;
    int y;

    /* Calculate the visible scape tiles. */
    world_view_chunks(WORLD_TILE_DIM, WORLD_TILE_DIM, &chunk_rsp);

    canvasy = chunk_rsp.sub_offy;
    for (y = 0; y < chunk_rsp.num_chunksy; ++y) {
        canvasx = chunk_rsp.sub_offx;
        for (x = 0; x < chunk_rsp.num_chunksx; ++x) {
            terr_id = terr_scape_get(chunk_rsp.chunkx + x,
                                     chunk_rsp.chunky + y);
            terr_draw(world_view_info.canvas, canvasx, canvasy, terr_id);

            canvasx += TILE_DIM;
        }
        canvasy += TILE_DIM;
    }
}

/*****************************************************************************
 * $init                                                                     *
 *****************************************************************************/

int
terr_init(void)
{
    pslab_create(&terr_data.live_pslab, sizeof (struct terr_live_t),
                 TERR_LIVE_MAX);
    return 0;
}


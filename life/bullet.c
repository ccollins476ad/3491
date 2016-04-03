#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "gen/gen_num.h"
#include "gen/container.h"
#include "gen/direction.h"
#include "gfx/balleg.h"
#include "gfx/gfx.h"
#include "gfx/canvas.h"
#include "gfx/icon.h"
#include "gfx/tile.h"
#include "life/world.h"
#include "life/zone.h"
#include "life/hit_box.h"
#include "life/collision.h"
#include "life/blast.h"
#include "life/spark.h"
#include "life/terr.h"
#include "life/stats.h"
#include "life/bullet_defs.h"
#include "life/bullet.h"

#define BULLET_MAX          2048

static struct {
    struct slab_t slab;
} bullet_data;


void
bullet_center_offset(struct bullet_properties_t *properties,
                     int *offsetx, int *offsety)
{
    /* Assume the first hit box is centered. */
    hit_box_desc_center(properties->hit_boxes, offsetx, offsety);
    *offsetx *= WORLD_FACTOR;
    *offsety *= WORLD_FACTOR;
}

static void
bullet_center(struct bullet_t *bullet, int *centerx, int *centery)
{
    int offsetx;
    int offsety;

    bullet_center_offset(bullet->properties, &offsetx, &offsety);

    *centerx = bullet->phys.x + offsetx;
    *centery = bullet->phys.y + offsety;
}

static void
bullet_set_smoke_rate(struct bullet_t *bullet)
{
    struct bullet_smoke_properties_t *smoke_properties;

    smoke_properties = bullet->properties->smoke_properties;
    bullet->smoke_ticker.rate = smoke_properties->rate_base +
                                rand_up_to(smoke_properties->rate_rand);
}

struct bullet_t *
bullet_add(struct bullet_properties_t *properties)
{
    struct bullet_t *bullet;

    bullet = slab_insert(&bullet_data.slab);
    if (bullet == NULL) {
        ++stats_data.bullet.oom;
        return NULL;
    }

    memset(bullet, 0, sizeof *bullet);
    bullet->entity.id = entity_next_id();
    bullet->entity.type = ENTITY_TYPE_BULLET;
    bullet->birth_tick = world_tick;
    bullet->properties = properties;

    if (properties->smoke_properties != NULL) {
        bullet_set_smoke_rate(bullet);
    }

    return bullet;
}

static void
bullet_build_hit_box(struct bullet_t *bullet, struct hit_box_desc_t *desc,
                     struct hit_box_t *hit_box)
{
    hit_box_from_desc(hit_box, desc,
                      bullet->phys.x, bullet->phys.y);
    hit_box->owner = &bullet->entity;
    hit_box_adjust_for_speed(hit_box,
                             bullet->phys.xspeed, bullet->phys.yspeed);
}

static void
bullet_erase(struct bullet_t *bullet)
{
    entity_trashed(&bullet->entity);
    combat_free_ignore_list(bullet->ignore_list);
    slab_erase_elem(&bullet_data.slab, bullet);
}

static int
bullet_in_zone(struct bullet_t *bullet)
{
    return bullet->phys.x >= zone_bulletx &&
           bullet->phys.y >= zone_bullety &&
           bullet->phys.x < zone_bulletx + zone_bullet_dimx &&
           bullet->phys.y < zone_bullety + zone_bullet_dimy;
}

static int
bullet_hit_box_terr(struct bullet_t *bullet, struct hit_box_t *hit_box)
{
    uint8_t allowed_dirs;
    int centerx;
    int centery;
    int diffx;
    int diffy;
    int colx;
    int coly;
    int colw;
    int colh;
    int rc;

    if (hit_box->flags & HIT_BOX_F_CLP) {
        rc = collision_terr_solid_box(hit_box, &colx, &coly, &colw, &colh, 0);
        if (rc != COLLISION_TYPE_NONE) {
            /* Bullet has hit solid terrain; apply damage to wall. */
            rc = combat_bullet_vs_wall(colx, coly, bullet);
            if (rc) {
                /* Wall fully absorbed bullet.  Move bullet out of wall and
                 * process the impact.
                 */
                allowed_dirs = 1 << DIRECTION_UP    | 1 << DIRECTION_RIGHT  |
                               1 << DIRECTION_DOWN  | 1 << DIRECTION_LEFT   |
                               1 << DIRECTION_UR    | 1 << DIRECTION_DR     |
                               1 << DIRECTION_DL    | 1 << DIRECTION_UL;

                rc = collision_terr_escape(hit_box, allowed_dirs,
                                           &diffx, &diffy);
                if (rc != COLLISION_TYPE_NONE) {
                    bullet->phys.x += diffx;
                    bullet->phys.y += diffy;
                }
                bullet_center(bullet, &centerx, &centery);
                bullet_impact(bullet, centerx, centery);
                return 1;
            }
        }
    }

    return 0;
}

static void
bullet_add_smoke(struct bullet_t *bullet)
{
    struct bullet_smoke_properties_t *smoke_properties;
    struct spark_t *spark;
    int frame_dur;

    smoke_properties = bullet->properties->smoke_properties;
    assert(smoke_properties != NULL);

    frame_dur = smoke_properties->frame_dur_base +
                rand_up_to(smoke_properties->frame_dur_rand);
    spark = spark_add(smoke_properties->spark_properties, frame_dur);
    if (spark != NULL) {
        spark->particle.phys.x = bullet->phys.x - bullet->phys.xspeed * 3;
        spark->particle.phys.y = bullet->phys.y - bullet->phys.yspeed * 3;
    }

    bullet_set_smoke_rate(bullet);
}

static void
bullet_update(struct bullet_t *bullet)
{
    struct hit_box_desc_t *desc;
    struct hit_box_t hit_box;
    int do_smoke;
    int rc;

    if (bullet->flags & BULLET_F_TRASH) {
        bullet_erase(bullet);
        return;
    }

    phys_update(&bullet->phys);
    if (!bullet_in_zone(bullet)) {
        bullet_erase(bullet);
        return;
    }

    HIT_BOX_DESCS_FOREACH(bullet->properties->hit_boxes, desc) {
        bullet_build_hit_box(bullet, desc, &hit_box);
        if (hit_box.flags & HIT_BOX_F_CLP) {
            rc = bullet_hit_box_terr(bullet, &hit_box);
            if (rc) {
                return;
            }
        }

        hit_box_add(&hit_box);
    }

    if (bullet->properties->smoke_properties != NULL) {
        do_smoke = ticker_update(&bullet->smoke_ticker, 1);
        if (do_smoke) {
            bullet_add_smoke(bullet);
        }
    }
}

void
bullet_update_all(void)
{
    struct bullet_t *bullet;

    SLAB_FOREACH(&bullet_data.slab, bullet) {
        bullet_update(bullet);
    }
}

void
bullet_draw_all(void)
{
    struct bullet_t *bullet;

    SLAB_FOREACH(&bullet_data.slab, bullet) {
        world_view_draw_icon(&bullet->properties->icon, NULL,
                             bullet->phys.x, bullet->phys.y);
    }
}

void
bullet_clear(void)
{
    struct bullet_t *bullet;

    SLAB_FOREACH(&bullet_data.slab, bullet) {
        bullet->flags |= BULLET_F_TRASH;
    }
}

void
bullet_shrapnel_add(int centerx, int centery, unsigned int owner_id,
                    struct shrapnel_properties_t *properties)
{
    struct bullet_t *bullet;
    int center_offsetx;
    int center_offsety;
    int count;
    int x;
    int y;
    int i;

    bullet_center_offset(properties->bullet_properties,
                         &center_offsetx, &center_offsety);

    x = centerx - center_offsetx;
    y = centery - center_offsety;

    count = properties->count_base + rand_up_to(properties->count_rand);

    for (i = 0; i < count; i++) {
        bullet = bullet_add(properties->bullet_properties);
        if (bullet == NULL) {
            break;
        }

        bullet->owner_id = owner_id;
        bullet->phys.x = x;
        bullet->phys.y = y;
        erupt_apply(&properties->erupt, &bullet->phys);

        if (bullet->properties->flags & BULLET_PROPERTIES_F_IGNORE_OWNER) {
            combat_ignore(&bullet->ignore_list, owner_id);
        }
    }
}

int
bullet_survives_strike(int survive_type, int was_dead, int is_dead)
{
    switch (survive_type) {
    case BULLET_STRIKE_SURVIVE_TYPE_NEVER:
        return 0;

    case BULLET_STRIKE_SURVIVE_TYPE_DEAD:
        return was_dead;

    case BULLET_STRIKE_SURVIVE_TYPE_KILL:
        return is_dead;

    case BULLET_STRIKE_SURVIVE_TYPE_ALWAYS:
        return 1;

    default:    
        assert(0);
        return 0;
    }
}

void
bullet_impact(struct bullet_t *bullet, int centerx, int centery)
{
    bullet->flags |= BULLET_F_TRASH;

    if (bullet->properties->blast_properties != NULL) {
        blast_apply(bullet->properties->blast_properties, centerx, centery,
                    bullet->owner_id);
    }
}

int
bullet_init(void)
{
    int rc;

    slab_destroy(&bullet_data.slab);

    rc = slab_create(&bullet_data.slab, sizeof (struct bullet_t), BULLET_MAX);
    if (rc != 0) {
        goto err;
    }

    return 0;

err:
    slab_destroy(&bullet_data.slab);

    return rc;
}


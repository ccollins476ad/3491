#include <assert.h>
#include <math.h>
#include <errno.h>
#include "gen/gen_dbg.h"
#include "gen/gen_num.h"
#include "gen/gen_shape.h"
#include "gen/direction.h"
#include "life/phys.h"
#include "gen/container.h"
#include "gfx/balleg.h"
#include "gfx/icon.h"
#include "gfx/tile.h"
#include "gfx/gfx.h"
#include "life/entity.h"
#include "life/world.h"
#include "life/collision.h"
#include "life/hit_box.h"
#include "life/being.h"
#include "life/fsm.h"
#include "life/life_defs.h"
#include "life/carnage.h"

#define CARNAGE_MAX         256

#define CARNAGE_SPEED_FRICTION    0.97
#define CARNAGE_ANGLE_FRICTION    0.96

#define CARNAGE_SPEED_INHERIT_FACTOR    0.35

/**
 * Fader settings that get applied on death.
 */
static struct fader_t carnage_death_fader = {
    .color_info = {
        .mode = COLOR_MODE_TRANS,
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255,
    },
    .delta = -5,
};

struct {
    struct slab_t slab;
} carnage_data;

static void
carnage_erase(struct carnage_t *carnage)
{
    if (carnage == NULL) {
        return;
    }
    destroy_bitmap(carnage->bmp);
    slab_erase_elem(&carnage_data.slab, carnage);
}

static void
carnage_draw(struct carnage_t *carnage)
{
    world_view_smart_draw(carnage->bmp, carnage->phys.x, carnage->phys.y,
                          &carnage->fader.color_info, 1.0,
                          carnage->angle / WORLD_FACTOR, 0, 0);
}

void
carnage_draw_all(void)
{
    struct carnage_t *carnage;

    SLAB_FOREACH(&carnage_data.slab, carnage) {
        carnage_draw(carnage);
    }
}

static void
carnage_update(struct carnage_t *carnage)
{
    struct hit_box_t hit_box;
    int diffx;
    int diffy;
    int dir;
    int rc;

    if (world_tick == carnage->expiry) {
        carnage->fader = carnage_death_fader;
    }
    if (carnage->fader.color_info.mode != COLOR_MODE_NONE &&
        carnage->fader.color_info.a == 0) {

        carnage_erase(carnage);
        return;
    }

    phys_apply_friction(&carnage->phys, CARNAGE_SPEED_FRICTION);

    phys_update(&carnage->phys);
    carnage->angle_speed *= CARNAGE_ANGLE_FRICTION;
    carnage->angle += carnage->angle_speed;
    while (carnage->angle < 0) {
        carnage->angle += 359 * WORLD_FACTOR;
    }
    while (carnage->angle > 359 * WORLD_FACTOR) {
        carnage->angle -= 359 * WORLD_FACTOR;
    }

    fader_update(&carnage->fader);

    hit_box.flags = HIT_BOX_F_CLP;
    hit_box.shape.type = SHAPE_TYPE_RECTANGLE;
    hit_box.shape.rectangle.x = carnage->phys.x;
    hit_box.shape.rectangle.y = carnage->phys.y;
    hit_box.shape.rectangle.dimx = carnage->bmp->w * WORLD_FACTOR;
    hit_box.shape.rectangle.dimy = carnage->bmp->h * WORLD_FACTOR;
    hit_box.owner = &carnage->entity;

    rc = collision_terr_hit_box_once(&hit_box, &diffx, &diffy);
    if (rc != COLLISION_TYPE_NONE) {
        carnage->phys.x += diffx;
        carnage->phys.y += diffy;
        dir = collision_terr_impact(carnage->phys.xspeed, carnage->phys.yspeed,
                                    diffx, diffy);
        switch (dir) {
        case FSM_SIG_NONE:
            break;

        case DIRECTION_LEFT:
        case DIRECTION_RIGHT:
            phys_apply_friction(&carnage->phys, LIFE_BOUNCE_FRICTION);
            carnage->angle_speed *= LIFE_BOUNCE_FRICTION;
            carnage->phys.xspeed *= -1;
            break;

        case DIRECTION_UP:
        case DIRECTION_DOWN:
            phys_apply_friction(&carnage->phys, LIFE_BOUNCE_FRICTION);
            carnage->angle_speed *= LIFE_BOUNCE_FRICTION;
            carnage->phys.yspeed *= -1;
            break;

        default:
            assert(0);
            break;
        }
    }
}

void
carnage_update_all(void)
{
    struct carnage_t *carnage;

    SLAB_FOREACH(&carnage_data.slab, carnage) {
        carnage_update(carnage);
    }
}

void
carnage_clear(void)
{
    struct carnage_t *carnage;

    SLAB_FOREACH(&carnage_data.slab, carnage) {
        carnage_erase(carnage);
    }
}

void
carnage_signal(struct carnage_t *carnage, struct entity_t *sender,
               uint8_t signal)
{
    struct being_t *owner;

    /* Carnage should only listen to owning being; assume sender is owner. */
    switch (signal) {
    case ENTITY_SIGNAL_TRASH:
        /* Inherit owner's speed. */
        owner = entity_to_being(sender);
        carnage->phys.xspeed += owner->phys.xspeed *
                                CARNAGE_SPEED_INHERIT_FACTOR;
        carnage->phys.yspeed += owner->phys.yspeed *
                                CARNAGE_SPEED_INHERIT_FACTOR;
        break;

    default:
        assert(0);
        break;
    }
}

static void
carnage_draw_point(struct BITMAP *bmp, int x, int y)
{
    uint32_t color;

    if (!is_inside_bitmap(bmp, x, y, 0)) {
        return;
    }

#if 0
    color = getpixel(bmp, x, y);
    if (color == COLOR_SPRITE_TRANS) {
        return;
    }
#endif

    color = RED(70 + rand_up_to(130));
    putpixel(bmp, x, y, color);
}

static void
carnage_draw_blood_splotch(struct BITMAP *bmp, int x, int y)
{
    int *vector;
    int dir;

    /* Always draw in-line point. */
    carnage_draw_point(bmp, x, y);

    /* Randomly draw adjacent points. */
    for (dir = 0; dir < DIRECTION_MAX; dir++) {
        if (rand_up_to(100) < 50) {
            vector = direction_to_vector(dir);
            carnage_draw_point(bmp, x + vector[0], y + vector[1]);
        }
    }
}

static void
carnage_draw_blood_edge_cb(struct BITMAP *bmp, int x, int y, int unused)
{
    carnage_draw_blood_splotch(bmp, x, y);
}

static void
carnage_draw_blood_edge(struct carnage_t *carnage, int x1, int y1,
                        int x2, int y2)
{
    do_line(carnage->bmp, x1, y1, x2, y2, 0, carnage_draw_blood_edge_cb);
}

static struct carnage_t *
carnage_new(void)
{
    struct carnage_t *carnage;

    carnage = slab_insert(&carnage_data.slab);
    if (carnage == NULL) {
        return NULL;
    }
    memset(carnage, 0, sizeof *carnage);
    carnage->entity.type = ENTITY_TYPE_CARNAGE;
    carnage->entity.id = entity_next_id();

    return carnage;
}

static struct BITMAP *
carnage_create_bitmap(int dimx, int dimy)
{
    struct BITMAP *bmp;

    bmp = create_bitmap(dimx + 2, dimy + 2);
    assert(bmp != NULL);
    clear_to_color(bmp, COLOR_TRANS);

    return bmp;
}

/**
 * @return          0 on success;
 *                  nonzero if the carnage object could not be populated due
 *                  to OOM, or because the cross dimensions didn't work out.
 */
static int
carnage_fill_bits(struct carnage_t *carnage, struct BITMAP *src,
                  int x1, int y1, int dimx, int dimy)
{
    if (dimx == 0 || dimy == 0) {
        return -1;
    }

    /* Add two to both dimensions to create a one-pixel buffer surrounding
     * the image.  This will allow blood to be placed past each edge.
     */
    carnage->bmp = carnage_create_bitmap(dimx + 2, dimy + 2);
    blit(src, carnage->bmp, x1, y1, 1, 1, dimx, dimy);

    /* All of these lines start at an offset of 1, and extend to the full
     * length of the bitmap's appropriate dimension.  Normally, they would
     * start at 0, and go to dim-1, but the 1-pixel buffer adjusts the line
     * endpoint offsets.
     */
    if (x1 != 0) {
        carnage_draw_blood_edge(carnage, 1, 1, 1, dimy);
    }
    if (x1 + dimx != src->w) {
        carnage_draw_blood_edge(carnage, dimx, 1, dimx, dimy);
    }
    if (y1 != 0) {
        carnage_draw_blood_edge(carnage, 1, 1, dimx, 1);
    }
    if (y1 + dimy != src->h) {
        carnage_draw_blood_edge(carnage, 1, dimy, dimx, dimy);
    }

    carnage->phys.x = (x1 - 1) * WORLD_FACTOR;
    carnage->phys.y = (y1 - 1) * WORLD_FACTOR;

    return 0;
}

/**
 * Attempts to add four bits-type carnages.
 *
 * @param icon          The icon to build carnage from.
 * @param xoff          The X-offset of the cross center.
 * @param yoff          The Y-offset of the cross center.
 * @param dst           Array of pointers; on success, pointers to the new
 *                      carnages get written here.
 * @param dst_len       Input:  Size of the "dst" array.
 *                      Output: Number of caranges inserted into the 'dst'
 *                              array.  On failure, this is 0.
 */
void
carnage_add_bits(struct icon_t *icon, int xoff, int yoff,
                 struct carnage_t **dst, int *dst_len)
{
    struct carnage_t *carnage;
    struct BITMAP *full;
    int rc;
    int i;

    full = tile_to_bitmap(icon->tile_id, icon->fg);

    for (i = 0; i < 4 && i < *dst_len; i++) {
        carnage = carnage_new();
        dst[i] = carnage;
        if (carnage == NULL) {
            goto err;
        }

        switch (i) {
        case 0:
            rc = carnage_fill_bits(carnage, full, 0, 0, xoff, yoff);
            carnage->rads_off = 0.75 * M_PI;
            break;
        case 1:
            rc = carnage_fill_bits(carnage, full, xoff, 0, TILE_DIM - xoff,
                                   yoff);
            carnage->rads_off = 0.25 * M_PI;
            break;
        case 2:
            rc = carnage_fill_bits(carnage, full, xoff, yoff,
                                   TILE_DIM - xoff, TILE_DIM - yoff);
            carnage->rads_off = 1.75 * M_PI;
            break;
        case 3:
            rc = carnage_fill_bits(carnage, full, 0, yoff, xoff,
                                   TILE_DIM - yoff);
            carnage->rads_off = 1.25;
            break;
        default:
            assert(0);
            rc = -1;
        }

        if (rc != 0) {
            goto err;
        }
    }

    *dst_len = i;
    destroy_bitmap(full);

    return;

err:
    for (; i >= 0; i--) {
        carnage_erase(dst[i]);
    }

    *dst_len = 0;
    destroy_bitmap(full);
}

struct carnage_slice_row_t {
    int srcx;
    int srcy;
    int dstx;
    int dsty;
    int width;
};

struct carnage_slice_info_t {
    struct carnage_slice_row_t rows[256];
    int width;
    int height;
    int xoff;
    int yoff;
    int bloodx1;
    int bloody1;
    int bloodx2;
    int bloody2;
};

static void
carnage_slice_fill_info(struct carnage_slice_info_t *info,
                        struct BITMAP *src,
                        struct line_t *line, int side)
{
    struct carnage_slice_row_t *row;
    int intersections[2][2];
    int num_intersections;
    int found;
    int dsty;
    int x;
    int i;

    assert(isnormal(line->slope));

    found = 0;
    info->width = 0;
    info->height = 0;
    info->xoff = INT_MAX;
    info->yoff = 0;
    dsty = 0;
    for (i = 0; i < src->h; i++) {
        row = info->rows + info->height;

        x = line_x_given_y(line, i);
        if (x < 0) {
            if (side == 0) {
                memset(row, 0, sizeof *row);
            } else {
                row->srcx = 0;
                row->width = src->w;
            }
        } else if (x >= src->w) {
            if (side == 0) {
                row->srcx = 0;
                row->width = src->w;
            } else {
                memset(row, 0, sizeof *row);
            }
        } else {
            /* Intersection gets included in both sides. */
            if (side == 0) {
                row->srcx = 0;
                row->width = x;
            } else {
                row->srcx = x;
                row->width = src->w - x;
            }

            found = 1;
        }

        if (row->width > 0) {
            if (info->height == 0) {
                /* First row. */
                info->yoff = i;
            }

            row->srcy = i;
            row->dsty = dsty++;
            info->width = max(info->width, row->width);
            info->height++;
            info->xoff = min(info->xoff, row->srcx);
        }
    }

    /* Second pass to determine destination x values. */
    for (i = 0; i < info->height; i++) {
        row = info->rows + i;
        row->dstx = row->srcx - info->xoff;
    }

    num_intersections =
        line_rect_intersection_points(line, 0, 0, src->w, src->h,
                                      intersections);
    assert(num_intersections == 2);
    info->bloodx1 = intersections[0][0] - info->xoff;
    info->bloody1 = intersections[0][1] - info->yoff;
    info->bloodx2 = intersections[1][0] - info->xoff;
    info->bloody2 = intersections[1][1] - info->yoff;
}

static int
carnage_slice_try_fill_cardinal(struct carnage_t *carnage, struct BITMAP *src,
                                struct line_t *line, int side)
{
    int dimx;
    int dimy;
    int x1;
    int y1;
    int rc;

    if (line->slope == 0.0) {
        /* Horizontal slice. */
        x1 = 0;
        dimx = src->w;
        if (side == 0) {
            y1 = 0;
            dimy = line->offset;
        } else { 
            y1 = line->offset;
            dimy = src->h - line->offset;
        }
    } else if (isinf(line->slope)) {
        /* Vertical slice. */
        y1 = 0;
        dimy = src->h;
        if (side == 0) {
            x1 = 0;
            dimx = line->offset;
        } else { 
            x1 = line->offset;
            dimx = src->w - line->offset;
        }
    } else {
        return EAGAIN;
    }

    rc = carnage_fill_bits(carnage, src, x1, y1, dimx, dimy);
    return rc;
}

static double
carnage_slice_calc_rads_off(double slope, int side)
{
    double rads_slope;
    double rads_orth;

    rads_slope = rads_from_slope(slope);
    if (side == 0) {
        rads_orth = rads_normalize(rads_slope + 0.5 * M_PI);
    } else {
        rads_orth = rads_normalize(rads_slope - 0.5 * M_PI);
    }

    return rads_orth;
}

static int
carnage_slice_fill(struct carnage_t *carnage, struct BITMAP *src,
                   struct line_t *line, int side)
{
    struct carnage_slice_info_t info;
    struct carnage_slice_row_t *row;
    int rc;
    int i;

    /* Try a simple cardinal slice first. */
    rc = carnage_slice_try_fill_cardinal(carnage, src, line, side);
    if (rc == 0) {
        carnage->rads_off = carnage_slice_calc_rads_off(line->slope, side);
        return 0;
    } else if (rc != EAGAIN) {
        /* Error. */
        return rc;
    }

    /* Couldn't do a cardinal slice. */

    carnage_slice_fill_info(&info, src, line, side);
    carnage->bmp = carnage_create_bitmap(info.width, info.height);

    for (i = 0; i < info.height; i++) {
        row = info.rows + i;
        if (row->width > 0) {
            assert(row->srcx >= 0 && row->srcx + row->width <= src->w);
            assert(row->srcy >= 0 && row->srcy < src->h);
            assert(row->dstx >= 0 && row->dstx + row->width <= info.width);
            assert(row->dsty >= 0 && row->dsty < info.height);

            blit(src, carnage->bmp,
                 row->srcx, row->srcy,
                 row->dstx, row->dsty,
                 row->width, 1);
        }
    }

    carnage_draw_blood_edge(carnage, info.bloodx1, info.bloody1,
                            info.bloodx2, info.bloody2);

    carnage->phys.x = info.xoff * WORLD_FACTOR;
    carnage->phys.y = info.yoff * WORLD_FACTOR;

    carnage->rads_off = carnage_slice_calc_rads_off(line->slope, side);

    return 0;
}

/**
 * Attempts to add two slice-type carnages.
 *
 * @param icon          The icon to build carnage from.
 * @param line          The intersecting line.
 * @param dst           Array of pointers; on success, pointers to the new
 *                      carnages get written here.
 * @param dst_len       Input:  Size of the "dst" array.
 *                      Output: Number of caranges inserted into the 'dst'
 *                              array.  On failure, this is 0.
 */
void
carnage_slice_add(struct icon_t *icon, struct line_t *line,
                  struct carnage_t **dst, int *dst_len)
{
    struct carnage_t *carnage;
    struct BITMAP *full;
    int intersections[2][2];
    int num_intersections;
    int rc;
    int i;

    /* If slice is on the edge, nudge it towards the center. */
    line_rect_ensure_margin(line, 0, 0, TILE_DIM, TILE_DIM, 1);

    num_intersections =
        line_rect_intersection_points(line, 0, 0, TILE_DIM, TILE_DIM,
                                      intersections);
    if (num_intersections != 2) {
        *dst_len = 0;
        return;
    }

    full = tile_to_bitmap(icon->tile_id, icon->fg);

    for (i = 0; i < 2 && i < *dst_len; i++) {
        carnage = carnage_new();
        dst[i] = carnage;
        if (carnage == NULL) {
            goto err;
        }

        rc = carnage_slice_fill(carnage, full, line, i);
        if (rc != 0) {
            goto err;
        }

    }

    *dst_len = i;
    destroy_bitmap(full);

    return;

err:
    for (; i >= 0; i--) {
        carnage_erase(dst[i]);
    }

    *dst_len = 0;
    destroy_bitmap(full);
}

int
carnage_mass(struct carnage_t *carnage)
{
    return carnage->bmp->w * carnage->bmp->h;
}

void
carnage_init(void)
{
    int rc;

    slab_destroy(&carnage_data.slab);

    rc = slab_create(&carnage_data.slab, sizeof (struct carnage_t),
                     CARNAGE_MAX);
    assert(rc == 0);
}


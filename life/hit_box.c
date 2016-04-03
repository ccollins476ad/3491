#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "gen/gen_dbg.h"
#include "gen/gen_num.h"
#include "gen/container.h"
#include "gfx/balleg.h"
#include "gfx/gfx.h"
#include "life/world.h"
#include "life/stats.h"
#include "life/hit_box.h"

#define HIT_BOX_ATK_MAX         2048
#define HIT_BOX_DEF_MAX         64
#define HIT_BOX_CLP_MAX         2048
#define HIT_BOX_USE_MAX         64
#define HIT_BOX_TCH_MAX         64
#define HIT_BOX_SLD_MAX         256
#define HIT_BOX_MAX             HIT_BOX_ATK_MAX

#define HIT_BOX_CELLSX          4
#define HIT_BOX_CELLSY          4

#define HIT_BOX_INTERSECTION_MAX   1024

struct hit_box_cell_t {
    struct pslab_t atk_ptrs;
    struct pslab_t def_ptrs;
    struct pslab_t clp_ptrs;
    struct pslab_t use_ptrs;
    struct pslab_t tch_ptrs;
    struct pslab_t sld_ptrs;
};

static struct {
    struct hit_box_cell_t cells[HIT_BOX_CELLSY][HIT_BOX_CELLSX];
    int cell_dimx;
    int cell_dimy;

    int x;
    int y;
    int dimx;
    int dimy;
} hit_box_data;

struct pslab_t hit_box_boxes;
struct pslab_t hit_box_intersections;

static struct pslab_t *
hit_box_pslab_from_type(struct hit_box_cell_t *cell, uint8_t hit_box_type)
{
    switch (hit_box_type) {
    case HIT_BOX_F_CLP:
        return &cell->clp_ptrs;

    case HIT_BOX_F_ATK:
        return &cell->atk_ptrs;

    case HIT_BOX_F_DEF:
        return &cell->def_ptrs;

    case HIT_BOX_F_USE:
        return &cell->use_ptrs;

    case HIT_BOX_F_TCH:
        return &cell->tch_ptrs;

    case HIT_BOX_F_SLD:
        return &cell->sld_ptrs;

    default:
        assert(0);
        return NULL;
    }
}

void
hit_box_clear(void)
{
    int x;
    int y;

    for (y = 0; y < HIT_BOX_CELLSY; ++y) {
        for (x = 0; x < HIT_BOX_CELLSX; ++x) {
            hit_box_data.cells[y][x].atk_ptrs.size = 0;
            hit_box_data.cells[y][x].def_ptrs.size = 0;
            hit_box_data.cells[y][x].clp_ptrs.size = 0;
            hit_box_data.cells[y][x].use_ptrs.size = 0;
            hit_box_data.cells[y][x].tch_ptrs.size = 0;
            hit_box_data.cells[y][x].sld_ptrs.size = 0;
        }
    }

    hit_box_boxes.size = 0;
}

static int
hit_box_cell_indices_point(struct point_t *point,
                           int *dst_x1, int *dst_y1,
                           int *dst_x2, int *dst_y2)
{
    int xoff;
    int yoff;
    int x1;
    int y1;

    xoff = point->x - hit_box_data.x;
    x1 = xoff / hit_box_data.cell_dimx;

    yoff = point->y - hit_box_data.y;
    y1 = yoff / hit_box_data.cell_dimy;

    if (x1 >= HIT_BOX_CELLSX || y1 >= HIT_BOX_CELLSY) {
        return -1;
    }

    if (x1 < 0) {
        x1 = 0;
    }
    if (y1 < 0) {
        y1 = 0;
    }

    *dst_x1 = x1;
    *dst_y1 = y1;
    *dst_x2 = x1;
    *dst_y2 = y1;

    return 0;
}

static int
hit_box_cell_indices_rectangle(struct rectangle_t *rectangle,
                               int *dst_x1, int *dst_y1,
                               int *dst_x2, int *dst_y2)
{
    int xoff;
    int yoff;
    int x1;
    int y1;
    int x2;
    int y2;

    xoff = rectangle->x - hit_box_data.x;
    x1 = xoff / hit_box_data.cell_dimx;
    x2 = x1 + (xoff + rectangle->dimx +
               hit_box_data.cell_dimx - 1) / hit_box_data.cell_dimx;

    yoff = rectangle->y - hit_box_data.y;
    y1 = yoff / hit_box_data.cell_dimy;
    y2 = y1 + (yoff + rectangle->dimy +
               hit_box_data.cell_dimy - 1) / hit_box_data.cell_dimy;

    if (x2 <= 0 || x1 >= HIT_BOX_CELLSX || y2 <= 0 || y1 >= HIT_BOX_CELLSY) {
        return -1;
    }

    if (x1 < 0) {
        x1 = 0;
    }
    if (y1 < 0) {
        y1 = 0;
    }
    if (x2 > HIT_BOX_CELLSX) {
        x2 = HIT_BOX_CELLSX;
    }
    if (y2 > HIT_BOX_CELLSY) {
        y2 = HIT_BOX_CELLSY;
    }

    *dst_x1 = x1;
    *dst_y1 = y1;
    *dst_x2 = x2;
    *dst_y2 = y2;

    return 0;
}

static int
hit_box_cell_indices_circle(struct circle_t *circle,
                            int *dst_x1, int *dst_y1,
                            int *dst_x2, int *dst_y2)
{
    struct rectangle_t rectangle;

    rectangle.x = circle->x - circle->radius;
    rectangle.y = circle->y - circle->radius;
    rectangle.dimx = circle->radius * 2;
    rectangle.dimy = circle->radius * 2;

    return hit_box_cell_indices_rectangle(&rectangle, dst_x1, dst_y1,
                                          dst_x2, dst_y2);
}

static int
hit_box_cell_indices(struct hit_box_t *hit_box,
                     int *dst_x1, int *dst_y1,
                     int *dst_x2, int *dst_y2)
{
    switch (hit_box->shape.type) {
    case SHAPE_TYPE_POINT:
        return hit_box_cell_indices_point(&hit_box->shape.point,
                                          dst_x1, dst_y1, dst_x2, dst_y2);

    case SHAPE_TYPE_RECTANGLE:
        return hit_box_cell_indices_rectangle(&hit_box->shape.rectangle,
                                              dst_x1, dst_y1, dst_x2, dst_y2);

    case SHAPE_TYPE_CIRCLE:
        return hit_box_cell_indices_circle(&hit_box->shape.circle,
                                           dst_x1, dst_y1, dst_x2, dst_y2);

    default:
        assert(0);
        return -1;
    }
}

static int
hit_box_cell_add(struct hit_box_cell_t *cell, struct hit_box_t *hit_box)
{
    struct hit_box_t **hb_ptr;
    int rc;

    rc = ENOMEM;

    if (hit_box->flags & HIT_BOX_F_ATK) {
        hb_ptr = pslab_insert(&cell->atk_ptrs);
        if (hb_ptr == NULL) {
            stats_data.hit_box.oom++;
            stats_data.hit_box.oom_atk++;
        } else {
            *hb_ptr = hit_box;
            rc = 0;
        }
    }
    if (hit_box->flags & HIT_BOX_F_DEF) {
        hb_ptr = pslab_insert(&cell->def_ptrs);
        if (hb_ptr == NULL) {
            stats_data.hit_box.oom++;
            stats_data.hit_box.oom_def++;
        } else {
            *hb_ptr = hit_box;
            rc = 0;
        }
    }
    if (hit_box->flags & HIT_BOX_F_CLP) {
        hb_ptr = pslab_insert(&cell->clp_ptrs);
        if (hb_ptr == NULL) {
            stats_data.hit_box.oom++;
            stats_data.hit_box.oom_clp++;
        } else {
            *hb_ptr = hit_box;
            rc = 0;
        }
    }
    if (hit_box->flags & HIT_BOX_F_USE) {
        hb_ptr = pslab_insert(&cell->use_ptrs);
        if (hb_ptr == NULL) {
            stats_data.hit_box.oom++;
            stats_data.hit_box.oom_use++;
        } else {
            *hb_ptr = hit_box;
            rc = 0;
        }
    }
    if (hit_box->flags & HIT_BOX_F_TCH) {
        hb_ptr = pslab_insert(&cell->tch_ptrs);
        if (hb_ptr == NULL) {
            stats_data.hit_box.oom++;
            stats_data.hit_box.oom_tch++;
        } else {
            *hb_ptr = hit_box;
            rc = 0;
        }
    }
    if (hit_box->flags & HIT_BOX_F_SLD) {
        hb_ptr = pslab_insert(&cell->sld_ptrs);
        if (hb_ptr == NULL) {
            stats_data.hit_box.oom++;
            stats_data.hit_box.oom_sld++;
        } else {
            *hb_ptr = hit_box;
            rc = 0;
        }
    }

    return rc;
}

struct hit_box_t *
hit_box_add(struct hit_box_t *hit_box)
{
    struct hit_box_t *dst;
    int cellx1;
    int celly1;
    int cellx2;
    int celly2;
    int rc;
    int x;
    int y;

    if (!(hit_box->flags & HIT_BOX_F_ALL)) {
        return NULL;
    }

    rc = hit_box_cell_indices(hit_box, &cellx1, &celly1, &cellx2, &celly2);
    if (rc != 0) {
        /* Out of bounds. */
        return NULL;
    }

    dst = pslab_insert(&hit_box_boxes);
    if (dst == NULL) {
        return NULL;
    }
    *dst = *hit_box;

    for (y = celly1; y < celly2; ++y) {
        for (x = cellx1; x < cellx2; ++x) {
            rc = hit_box_cell_add(&hit_box_data.cells[y][x], dst);
            if (rc != 0) {
                return NULL;
            }
        }
    }

    return dst;
}

void
hit_box_desc_center(struct hit_box_desc_t *desc, int *centerx, int *centery)
{
    switch (desc->shape.type) {
    case SHAPE_TYPE_POINT:
        *centerx = desc->shape.point.x;
        *centery = desc->shape.point.y;
        break;

    case SHAPE_TYPE_RECTANGLE:
        *centerx = desc->shape.rectangle.x + desc->shape.rectangle.dimx / 2;
        *centery = desc->shape.rectangle.y + desc->shape.rectangle.dimy / 2;
        break;

    case SHAPE_TYPE_CIRCLE:
        *centerx = desc->shape.circle.x;
        *centery = desc->shape.circle.y;
        break;

    default:
        assert(0);
        break;
    }
}

void
hit_box_from_desc(struct hit_box_t *hit_box,
                  struct hit_box_desc_t *desc,
                  int worldx, int worldy)
{
    hit_box->shape.type = desc->shape.type;
    hit_box->flags = desc->flags;

    switch (desc->shape.type) {
    case SHAPE_TYPE_POINT:
        hit_box->shape.point.x = worldx + desc->shape.point.x * WORLD_FACTOR;
        hit_box->shape.point.y = worldy + desc->shape.point.y * WORLD_FACTOR;
        break;

    case SHAPE_TYPE_RECTANGLE:
        hit_box->shape.rectangle.x = worldx +
                                     desc->shape.rectangle.x * WORLD_FACTOR;
        hit_box->shape.rectangle.y = worldy +
                                     desc->shape.rectangle.y * WORLD_FACTOR;
        hit_box->shape.rectangle.dimx = desc->shape.rectangle.dimx *
                                        WORLD_FACTOR;
        hit_box->shape.rectangle.dimy = desc->shape.rectangle.dimy *
                                        WORLD_FACTOR;
        break;

    case SHAPE_TYPE_CIRCLE:
        hit_box->shape.circle.x = worldx + desc->shape.circle.x * WORLD_FACTOR;
        hit_box->shape.circle.y = worldy + desc->shape.circle.y * WORLD_FACTOR;
        hit_box->shape.circle.radius = desc->shape.circle.radius *
                                       WORLD_FACTOR;
        break;

    default:
        assert(0);
        break;
    }
}

void
hit_box_adjust_for_speed(struct hit_box_t *hit_box, int speedx, int speedy)
{
    if (hit_box->shape.type != SHAPE_TYPE_RECTANGLE) {
        return;
    }

    hit_box->shape.rectangle.dimx += abs(speedx);
    if (speedx < 0) {
        hit_box->shape.rectangle.x += speedx;
    }

    hit_box->shape.rectangle.dimy += abs(speedy);
    if (speedy < 0) {
        hit_box->shape.rectangle.y += speedy;
    }
}

static uint32_t
hit_box_color(struct hit_box_t *hit_box)
{

    int r;
    int g;
    int b;

    r = hit_box->flags & HIT_BOX_F_ATK                      ? 255 : 0;
    g = hit_box->flags & HIT_BOX_F_CLP                      ? 255 : 0;
    b = hit_box->flags & HIT_BOX_F_DEF                      ? 255 : 0;

    return COLOR(r, g, b);
}

static void
hit_box_draw_point(struct hit_box_t *hit_box)
{
    world_view_draw_point(hit_box->shape.point.x,
                          hit_box->shape.point.y,
                          hit_box_color(hit_box));
}

static void
hit_box_draw_rectangle(struct hit_box_t *hit_box)
{
    world_view_draw_rect(hit_box->shape.rectangle.x,
                         hit_box->shape.rectangle.y,
                         hit_box->shape.rectangle.dimx,
                         hit_box->shape.rectangle.dimy,
                         hit_box_color(hit_box));
}

static void
hit_box_draw_circle(struct hit_box_t *hit_box)
{
    world_view_draw_circle(hit_box->shape.circle.x,
                           hit_box->shape.circle.y,
                           hit_box->shape.circle.radius,
                           hit_box_color(hit_box));
}

typedef void hit_box_draw_fn_t(struct hit_box_t *hit_box);

static hit_box_draw_fn_t *hit_box_draw_fns[SHAPE_TYPE_MAX] = {
    [SHAPE_TYPE_POINT] = hit_box_draw_point,
    [SHAPE_TYPE_RECTANGLE] = hit_box_draw_rectangle,
    [SHAPE_TYPE_CIRCLE] = hit_box_draw_circle,
};

static void
hit_box_draw(struct hit_box_t *hit_box)
{
    hit_box_draw_fn_t *fn;

    fn = hit_box_draw_fns[hit_box->shape.type];
    if (fn != NULL) {
        fn(hit_box);
    }
}

void
hit_box_draw_all(void)
{
    struct hit_box_t *hit_box;
    int i;

    for (i = 0; i < hit_box_boxes.size; ++i) {
        hit_box = pslab_at(&hit_box_boxes, i);
        hit_box_draw(hit_box);
    }
}

static int
hit_box_pair_intersect(struct hit_box_t *lhs, struct hit_box_t *rhs)
{
    return shape_intersects(&lhs->shape, &rhs->shape);
}

static int
hit_box_already_processed(struct hit_box_t *atk, struct hit_box_t *def)
{
    struct hit_box_intersection_t *intersection;
    int i;

    for (i = 0; i < hit_box_intersections.size; ++i) {
        intersection = pslab_at(&hit_box_intersections, i);
        if (intersection->a == atk && intersection->b == def ||
            intersection->a == def && intersection->b == atk) {
            
            return 1;
        }
    }

    return 0;
}

static void
hit_box_process_cell(struct pslab_t *pslab_a, struct pslab_t *pslab_b)
{
    struct hit_box_intersection_t *intersection;
    struct hit_box_t **ab;
    struct hit_box_t **bb;
    int ai;
    int bi;

    for (bi = 0; bi < pslab_b->size; ++bi) {
        bb = pslab_at(pslab_b, bi);
        for (ai = 0; ai < pslab_a->size; ++ai) {
            ab = pslab_at(pslab_a, ai);
            /* Don't compare a hit box against itself. */
            if (ab != bb) {
                if (hit_box_pair_intersect(*bb, *ab)) {
                    if (!(hit_box_already_processed(*ab, *bb))) {
                        intersection = pslab_insert(&hit_box_intersections);
                        if (intersection != NULL) {
                            intersection->a = *ab;
                            intersection->b = *bb;
                        }
                    }
                }
            }
        }
    }
}

static void
hit_box_process_cells(int typea, int typeb)
{
    struct hit_box_cell_t *cell;
    struct pslab_t *pslaba;
    struct pslab_t *pslabb;
    int y;
    int x;

    hit_box_intersections.size = 0;

    for (y = 0; y < HIT_BOX_CELLSY; ++y) {
        for (x = 0; x < HIT_BOX_CELLSX; ++x) {
            cell = &hit_box_data.cells[y][x];

            pslaba = hit_box_pslab_from_type(cell, typea);
            pslabb = hit_box_pslab_from_type(cell, typeb);
            hit_box_process_cell(pslaba, pslabb);
        }
    }
}

void
hit_box_process_atk(void)
{
    hit_box_process_cells(HIT_BOX_F_ATK, HIT_BOX_F_DEF);
}

void
hit_box_process_tch(void)
{
    hit_box_process_cells(HIT_BOX_F_CLP, HIT_BOX_F_TCH);
}

int
hit_box_any_in_area(struct rectangle_t *area, uint8_t hit_box_flags)
{
    struct hit_box_cell_t *cell;
    struct hit_box_t **hb;
    struct hit_box_t area_hit_box;
    struct pslab_t *pslab;
    int cellx1;
    int celly1;
    int cellx2;
    int celly2;
    int rc;
    int x;
    int y;
    int i;

    /* Determine which cells need to be tested. */
    rc = hit_box_cell_indices_rectangle(area,
                                        &cellx1, &celly1, &cellx2, &celly2);
    if (rc != 0) {
        return 0;
    }

    /* Build a hit box from the supplied rectangle. */
    area_hit_box.shape.type = SHAPE_TYPE_RECTANGLE;
    area_hit_box.shape.rectangle = *area;
    for (y = celly1; y < celly2; y++) {
        for (x = cellx1; x < cellx2; x++) {
            cell = &hit_box_data.cells[y][x];
            pslab = hit_box_pslab_from_type(cell, hit_box_flags);
            for (i = 0; i < pslab->size; i++) {
                hb = pslab_at(pslab, i);
                if (hit_box_pair_intersect(&area_hit_box, *hb)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

struct hit_box_t *
hit_box_first_in_path(int x1, int y1, int x2, int y2, uint8_t hit_box_type)
{
    struct hit_box_cell_t *cell;
    struct hit_box_t *hit_box;
    struct pslab_t *pslab;
    int intersections[HIT_BOX_CELLSX + HIT_BOX_CELLSY][2];
    int num_intersections;
    int rc;
    int i;
    int h;

    num_intersections = sizeof intersections / sizeof intersections[0];
    rc = segment_tile_intersections(x1 - hit_box_data.x, y1 - hit_box_data.y,
                                    x2 - hit_box_data.x, y2 - hit_box_data.y,
                                    hit_box_data.cell_dimx,
                                    hit_box_data.cell_dimy,
                                    intersections, &num_intersections);
    assert(rc == 0);

    for (i = 0; i < num_intersections; i++) {
        cell = &hit_box_data.cells[intersections[i][1]][intersections[i][0]];
        pslab = hit_box_pslab_from_type(cell, hit_box_type);

        for (h = 0; h < pslab->size; h++) {
            hit_box = *(struct hit_box_t **)pslab_at(pslab, h);
            assert(hit_box->shape.type == SHAPE_TYPE_RECTANGLE);

            rc = seg_intersects_box(
                x1, y1, x2, y2,
                hit_box->shape.rectangle.x,
                hit_box->shape.rectangle.y,
                hit_box->shape.rectangle.x + hit_box->shape.rectangle.dimx,
                hit_box->shape.rectangle.y + hit_box->shape.rectangle.dimy);
            if (rc) {
                return hit_box;
            }
        }
    }

    return NULL;
}

void
hit_box_set_bounds(int x, int y, int dimx, int dimy)
{
    assert(dimx > 0 && dimy > 0);

    hit_box_clear();

    hit_box_data.x = x;
    hit_box_data.y = y;
    hit_box_data.dimx = dimx;
    hit_box_data.dimy = dimy;
    hit_box_data.cell_dimx = dimx / HIT_BOX_CELLSX;
    hit_box_data.cell_dimy = dimy / HIT_BOX_CELLSY;
}

void
hit_box_deinit(void)
{
    int x;
    int y;

    for (y = 0; y < HIT_BOX_CELLSY; ++y) {
        for (x = 0; x < HIT_BOX_CELLSX; ++x) {
            pslab_destroy(&hit_box_data.cells[y][x].atk_ptrs);
            pslab_destroy(&hit_box_data.cells[y][x].def_ptrs);
            pslab_destroy(&hit_box_data.cells[y][x].clp_ptrs);
            pslab_destroy(&hit_box_data.cells[y][x].use_ptrs);
            pslab_destroy(&hit_box_data.cells[y][x].tch_ptrs);
            pslab_destroy(&hit_box_data.cells[y][x].sld_ptrs);
        }
    }

    pslab_destroy(&hit_box_boxes);
    pslab_destroy(&hit_box_intersections);

    memset(&hit_box_data, 0, sizeof hit_box_data);
    memset(&hit_box_intersections, 0, sizeof hit_box_intersections);
}

void
hit_box_init(void)
{
    int rc;
    int x;
    int y;

    hit_box_deinit();

    rc = pslab_create(&hit_box_boxes, sizeof (struct hit_box_t),
                      HIT_BOX_MAX);
    assert(rc == 0);

    rc = pslab_create(&hit_box_intersections,
                      sizeof (struct hit_box_intersection_t),
                      HIT_BOX_INTERSECTION_MAX);
    assert(rc == 0);

    for (y = 0; y < HIT_BOX_CELLSY; ++y) {
        for (x = 0; x < HIT_BOX_CELLSX; ++x) {
            rc = pslab_create(&hit_box_data.cells[y][x].atk_ptrs,
                              sizeof (struct hit_box_t *), HIT_BOX_ATK_MAX);
            assert(rc == 0);

            rc = pslab_create(&hit_box_data.cells[y][x].def_ptrs,
                              sizeof (struct hit_box_t *), HIT_BOX_DEF_MAX);
            assert(rc == 0);

            rc = pslab_create(&hit_box_data.cells[y][x].clp_ptrs,
                              sizeof (struct hit_box_t *), HIT_BOX_CLP_MAX);
            assert(rc == 0);

            rc = pslab_create(&hit_box_data.cells[y][x].use_ptrs,
                              sizeof (struct hit_box_t *), HIT_BOX_USE_MAX);
            assert(rc == 0);

            rc = pslab_create(&hit_box_data.cells[y][x].tch_ptrs,
                              sizeof (struct hit_box_t *), HIT_BOX_TCH_MAX);
            assert(rc == 0);

            rc = pslab_create(&hit_box_data.cells[y][x].sld_ptrs,
                              sizeof (struct hit_box_t *), HIT_BOX_SLD_MAX);
            assert(rc == 0);
        }
    }
}


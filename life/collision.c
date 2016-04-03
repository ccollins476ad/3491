#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include "gen/gen_num.h"
#include "gen/gen_dbg.h"
#include "gen/direction.h"
#include "life/data.h"
#include "life/terr.h"
#include "life/world.h"
#include "life/hit_box.h"
#include "life/collision.h"

static int
collision_type(int all_solids, int tilex, int tiley)
{
    struct terr_properties_t *terr;
    struct rectangle_t rect;
    int tile_id;

    tile_id = terr_scape_get(tilex, tiley);
    terr = &data_terr[tile_id];

    if (terr->flags & TERR_F_SOLID) {
        return COLLISION_TYPE_IMPACT;
    }

    if (all_solids) {
        rect.x = tilex * WORLD_TILE_DIM;
        rect.y = tiley * WORLD_TILE_DIM;
        rect.dimx = WORLD_TILE_DIM;
        rect.dimy = WORLD_TILE_DIM;

        if (hit_box_any_in_area(&rect, HIT_BOX_F_SLD)) {
            return COLLISION_TYPE_IMPACT;
        }
    }

    return COLLISION_TYPE_NONE;
}

static int
collision_terr_solid_box_top(int all_solids, int hb_tilex, int hb_tiley,
                             int hb_tile_dimx, int hb_tile_dimy,
                             uint8_t hb_flags, int *x1, int *y1, int *dimx)
{
    int next_col_type;
    int col_type;
    int x;
    int y;

    col_type = COLLISION_TYPE_NONE;

    *x1 = 0;
    *y1 = 0;
    *dimx = 0;
    for (y = 0; y < hb_tile_dimy; ++y) {
        for (x = 0; x < hb_tile_dimx; ++x) {
            next_col_type = collision_type(all_solids, hb_tilex + x,
                                           hb_tiley + y);
            if (next_col_type != COLLISION_TYPE_NONE) {
                if (*dimx == 0) {
                    *x1 = x;
                    *y1 = y;
                }
                ++*dimx;
            } else {
                if (*dimx != 0) {
                    break;
                }
            }

            col_type = next_col_type;
        }

        if (*dimx != 0) {
            break;
        }
    }

    if (*dimx == 0) {
        col_type = COLLISION_TYPE_NONE;
    }
    return col_type;
}

static void
collision_terr_chunks(struct hit_box_t *hit_box, int *scapex1, int *scapey1,
                      int *scapedimx, int *scapedimy)
{
    struct calc_chunks_req_t req;
    struct calc_chunks_rsp_t rsp;

    switch (hit_box->shape.type) {
    case SHAPE_TYPE_POINT:
        world_view_tile_loc(hit_box->shape.point.x, hit_box->shape.point.y, 
                            scapex1, scapey1);
        *scapedimx = 1;
        *scapedimy = 1;
        break;

    case SHAPE_TYPE_RECTANGLE:
        req.x = hit_box->shape.rectangle.x;
        req.y = hit_box->shape.rectangle.y;
        req.dimx = hit_box->shape.rectangle.dimx;
        req.dimy = hit_box->shape.rectangle.dimy;
        req.chunk_dimx = WORLD_TILE_DIM;
        req.chunk_dimy = WORLD_TILE_DIM;
        calc_chunks(&req, &rsp);

        *scapex1 = rsp.chunkx;
        *scapey1 = rsp.chunky;
        *scapedimx = rsp.num_chunksx;
        *scapedimy = rsp.num_chunksy;
        break;

    default:
        assert(0);
        break;
    }
}

/**
 * Detects a collision between the supplied hit box and the landscape.
 *
 * @param hit_box
 * @param x1 On collision, the WORLD x of the left edge of the collision gets
 *           written here.
 * @param y1 On collision, the WORLD y of the left edge of the collision gets
 *           written here.
 * @param width On collision, the WORLD width of the intersection gets
 *              written here.
 * @param height On collision, the WORLD height of the intersection gets
 *               written here.
 *
 * @return A COLLISION_TYPE[...] constant.
 */
int
collision_terr_solid_box(struct hit_box_t *hit_box, int *x1, int *y1,
                         int *width, int *height, int all_solids)
{
    int scapedimx;
    int scapedimy;
    int scapex1;
    int scapey1;
    int next_col_type;
    int col_type;
    int x;
    int y;

    /* Determine which tiles the hit box intersects. */
    collision_terr_chunks(hit_box, &scapex1, &scapey1, &scapedimx, &scapedimy);

    col_type = collision_terr_solid_box_top(all_solids, scapex1, scapey1,
                                            scapedimx, scapedimy,
                                            hit_box->flags, x1, y1, width);
    if (col_type == COLLISION_TYPE_NONE) {
        return COLLISION_TYPE_NONE;
    }

    for (y = *y1 + 1; y < scapedimy; ++y) {
        for (x = *x1; x < *width; ++x) {
            next_col_type = collision_type(all_solids, x, y);
            if (next_col_type == COLLISION_TYPE_NONE) {
                goto done;
            }

            col_type = next_col_type;
        }
    }

done:
    *height = y - *y1;
    *x1 += scapex1;
    *y1 += scapey1;
    return col_type;
}

static int
collision_terr_is_hole(int x1, int y1, int width, int height)
{
    int col_type;
    int x;
    int y;

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            col_type = collision_type(1, x1 + x, y1 + y);
            if (col_type != COLLISION_TYPE_NONE) {
                return 0;
            }
        }
    }

    

    return 1;
}

/**
 * Retrieves the location of the nth tile edge in the direction specified.
 * This function operates across a single dimension.
 *
 * @param val The starting location.
 * @param dir_vect The movement direction (-1 or 1).
 * @param num_moves The number of tile edges to move. 
 *
 * @return The adjusted location.
 */
static int
collision_terr_adjust_point(int val, int dir_vect, int num_moves)
{
    int tile_val;

    if (num_moves > 0) {
        if (dir_vect < 0) {
            tile_val = (val - 1) / WORLD_TILE_DIM;
        } else {
            tile_val = val / WORLD_TILE_DIM + 1;
        }
        if (val < 0) {
            tile_val -= 1;
        }
        tile_val += dir_vect * (num_moves - 1);

        val = tile_val * WORLD_TILE_DIM;
    }

    return val;
}

/**
 * Retrieves the location of the supplied hit box after moving it past the
 * specified number of tile edges in a single direction. 
 *
 * @param hit_box
 * @param out_x The adjusted hit box's left edge gets written here. 
 * @param out_y The adjusted hit box's top edge gets written here. 
 * @param direction The movement direction.
 * @param num_moves The number of tile edges to move. 
 *
 * @return The adjusted location.
 */
static void
collision_terr_adjust_hit_box(struct hit_box_t *hit_box,
                              int *out_x, int *out_y,
                              int direction, int num_moves)
{
    int abs_num_movesx;
    int abs_num_movesy;
    int num_movesx;
    int num_movesy;
    int new_end;
    int end;

    assert(num_moves > 0);
    assert(hit_box->shape.type == SHAPE_TYPE_RECTANGLE);

    num_movesx = num_moves * direction_to_vector(direction)[0];
    num_movesy = num_moves * direction_to_vector(direction)[1];
    abs_num_movesx = abs(num_movesx);
    abs_num_movesy = abs(num_movesy);

    *out_x = hit_box->shape.rectangle.x;
    if (num_movesx < 0) {
        end = hit_box->shape.rectangle.x + hit_box->shape.rectangle.dimx;
        new_end = collision_terr_adjust_point(
            end, direction_to_vector(direction)[0], abs_num_movesx);
        *out_x += new_end - end;
    } else if (num_movesx > 0) {
        *out_x = collision_terr_adjust_point(
            *out_x, direction_to_vector(direction)[0], abs_num_movesx);
    }

    *out_y = hit_box->shape.rectangle.y;   
    if (num_movesy < 0) {
        end = hit_box->shape.rectangle.y + hit_box->shape.rectangle.dimy;
        new_end = collision_terr_adjust_point(
            end, direction_to_vector(direction)[1], abs_num_movesy);
        *out_y += new_end - end;
    } else if (num_movesy > 0) {
        *out_y = collision_terr_adjust_point(
            *out_y, direction_to_vector(direction)[1], abs_num_movesy);
    }
}

/** 
 * Determines if there is a large enough hole to accommodate the supplied hit
 * box after it is moved the specified distance and direction.
 */
static int
collision_terr_find_hole_once(struct hit_box_t *hit_box, int dir,
                              int num_moves, int *out_deltax, int *out_deltay)
{
    struct calc_chunks_req_t wvc_req;
    struct calc_chunks_rsp_t wvc_rsp;

    assert(hit_box->shape.type == SHAPE_TYPE_RECTANGLE);

    wvc_req.x = hit_box->shape.rectangle.x;
    wvc_req.y = hit_box->shape.rectangle.y;
    wvc_req.dimx = hit_box->shape.rectangle.dimx;
    wvc_req.dimy = hit_box->shape.rectangle.dimy;
    wvc_req.chunk_dimx = WORLD_TILE_DIM;
    wvc_req.chunk_dimy = WORLD_TILE_DIM;

    /* Calculate the hit box location if it were moved 'num_moves' tiles
     * in direction 'dir'.
     */
    collision_terr_adjust_hit_box(hit_box, &wvc_req.x, &wvc_req.y, dir,
                                  num_moves);

    /* Translate the hit box dimensions to tile coordinates. */
    calc_chunks(&wvc_req, &wvc_rsp);

    if (collision_terr_is_hole(
            wvc_rsp.chunkx, wvc_rsp.chunky,
            wvc_rsp.num_chunksx, wvc_rsp.num_chunksy)) {

        *out_deltax = wvc_req.x - hit_box->shape.rectangle.x;
        *out_deltay = wvc_req.y - hit_box->shape.rectangle.y;
        return 1;
    }

    return 0;
}

static int
collision_terr_test(struct hit_box_t *hit_box)
{
    int col_type;
    int temp[4];

    col_type = collision_terr_solid_box(
        hit_box, &temp[0], &temp[1], &temp[2], &temp[3], 1);

    return col_type;
}

/**
 * Detects collision between the specified hit box and the landscape, and
 * indicates the movement required to nullify the intersection.
 *
 * @param hit_box The hit box to test for collision.
 * @param allowed_dirs A bitmap indicating directions in which the hit box
 *                     should be allowed to "bounce out" of the intersection.
 *                     (directions are indicated via (1 << direction)).
 * @param out_deltax If collision is detected, the horizontal bounce vector gets
 *                   written here.
 * @param out_deltay If collision is detected, the vertical bounce vector gets
 *                   written here.
 *
 * @return COLLISION_TYPE_[...]
 */
int
collision_terr_escape(struct hit_box_t *hit_box, uint8_t allowed_dirs,
                      int *out_deltax, int *out_deltay)
{
    int sub_out_deltax;
    int sub_out_deltay;
    int num_moves;
    int min_dist;
    int col_type;
    int dist;
    int dir;
    int rc;

    /* First, check if there is even a collision at all. */
    col_type = collision_terr_test(hit_box);
    if (col_type == COLLISION_TYPE_NONE) {
        return COLLISION_TYPE_NONE;
    }

    /* Calculate distance to a suitably-sized hole in each of the allowed
     * directions.  The direction with the shortest distance wins.
     */
    *out_deltax = 0;
    *out_deltay = 0;
    num_moves = 1;
    min_dist = INT_MAX;
    while (*out_deltax == 0 && *out_deltay == 0) {
        for (dir = 0; dir < DIRECTION_MAX; ++dir) {
            if (allowed_dirs & 1 << dir) {
                rc = collision_terr_find_hole_once(
                    hit_box, dir, num_moves, &sub_out_deltax, &sub_out_deltay);
                if (rc) {
                    dist = distance(0, 0, sub_out_deltax, sub_out_deltay);
                    if (dist < min_dist) {
                        min_dist = dist;
                        *out_deltax = sub_out_deltax;
                        *out_deltay = sub_out_deltay;
                    }
                }
            }
        }
        ++num_moves;
    }

    return col_type;
}

int
collision_terr_hit_box_once(struct hit_box_t *hit_box, int *diffx, int *diffy)
{
    uint8_t allowed_dirs;
    int rc;

    allowed_dirs = 1 << DIRECTION_UP    | 1 << DIRECTION_RIGHT  |
                   1 << DIRECTION_DOWN  | 1 << DIRECTION_LEFT   |
                   1 << DIRECTION_UR    | 1 << DIRECTION_DR     |
                   1 << DIRECTION_DL    | 1 << DIRECTION_UL;
    *diffx = 0;
    *diffy = 0;

    rc = collision_terr_escape(hit_box, allowed_dirs, diffx, diffy);
    return rc;
}

int
collision_terr_impact(int xspeed, int yspeed, int diffx, int diffy)
{
    int direction;

    /* The relative magnitude of the two diffs indicate whether the impact was 
     * horizontal or vertical.  The sign of the relevant diff indicates the
     * specific direction.
     */
    direction = DIRECTION_NONE;
    if (abs(diffx) > abs(diffy)) {
        if (diffx > 0) {
            if (xspeed < 0) {
                direction = DIRECTION_LEFT;
            }
        } else if (diffx < 0) {
            if (xspeed > 0) {
                direction = DIRECTION_RIGHT;
            }
        }
    } else {
        if (diffy > 0) {
            if (yspeed < 0) {
                direction = DIRECTION_UP;
            }
        } else if (diffy < 0) {
            if (yspeed > 0) {
                direction = DIRECTION_DOWN;
            }
        }
    }

    return direction;
}

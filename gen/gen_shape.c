#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include "gen/gen_num.h"
#include "gen/gen_shape.h"

/*** @gen */

static int
shape_select_(int type1, int type2,
              struct shape_t *shape1, struct shape_t *shape2,
              void **out1, void **out2)
{
    if (shape1->type == type1 && shape2->type == type2) {
        *out1 = shape1;
        *out2 = shape2;
        return 0;
    }

    if (shape2->type == type1 && shape1->type == type1) {
        *out1 = shape2;
        *out2 = shape1;
        return 0;
    }

    return 1;
}

#define SHAPE_SELECT(type1, type2, shape1, shape2, out1, out2) do             \
{                                                                             \
    void *SS_v1;                                                              \
    void *SS_v2;                                                              \
    int SS_rc;                                                                \
                                                                              \
    SS_v1 = *(out1);                                                          \
    SS_v2 = *(out2);                                                          \
    SS_rc = shape_select_((type1), (type2), (shape1), (shape2),               \
                          &SS_v1, &SS_v2);                                    \
    assert(SS_rc == 0);                                                       \
    *(out1) = SS_v1;                                                          \
    *(out2) = SS_v2;                                                          \
} while (0)

/*** @intersects */

int
point_intersects_point(struct point_t *lhs, struct point_t *rhs)
{
    return lhs->x == rhs->x && lhs->y == rhs->y;
}

int
point_intersects_rectangle(struct point_t *point, struct rectangle_t *rect)
{
    return point->x >= rect->x && point->x < rect->x + rect->dimx &&
           point->y >= rect->y && point->y < rect->y + rect->dimy;
}

int
point_intersects_circle(struct point_t *point, struct circle_t *circle)
{
    int dist;

    dist = distance(point->x, point->y, circle->x, circle->y);
    return dist < circle->radius;
}

static int
rectangle_intersects_rectangle_raw(int r1x1, int r1y1, int r1w, int r1h,
                    int r2x1, int r2y1, int r2w, int r2h)
{
    int r1x2;
    int r1y2;
    int r2x2;
    int r2y2;

    r1x2 = r1x1 + r1w;
    r1y2 = r1y1 + r1h;
    r2x2 = r2x1 + r2w;
    r2y2 = r2y1 + r2h;

    return r1x1 < r2x2 && r1x2 > r2x1 && r1y1 < r2y2 && r1y2 > r2y1;
}

int
rectangle_intersects_rectangle(struct rectangle_t *r1, struct rectangle_t *r2)
{
    return rectangle_intersects_rectangle_raw(r1->x, r1->y, r1->dimx, r1->dimy,
                               r2->x, r2->y, r2->dimx, r2->dimy);
}

int
rectangle_intersects_circle(struct rectangle_t *rectangle,
                            struct circle_t *circle)
{
    int corner_dist_sq;
    int corner_dist_x;
    int corner_dist_y;
    int circle_dist_x;
    int circle_dist_y;

    circle_dist_x = abs(circle->x - rectangle->x);
    circle_dist_y = abs(circle->y - rectangle->y);

    if (circle_dist_x > rectangle->dimx / 2 + circle->radius) {
        return 0;
    }
    if (circle_dist_y > rectangle->dimy / 2 + circle->radius) {
        return 0;
    }

    if (circle_dist_x <= rectangle->dimx / 2) {
        return 1;
    }
    if (circle_dist_y <= rectangle->dimy / 2) {
        return 1;
    }

    corner_dist_x = circle_dist_x - rectangle->dimx / 2;
    corner_dist_y = circle_dist_y - rectangle->dimy / 2;
    corner_dist_sq = corner_dist_x * corner_dist_x +
                     corner_dist_y * corner_dist_y;

    if (corner_dist_sq <= circle->radius * circle->radius) {
        return 1;
    }

    return 0;
}

int
circle_intersects_circle(struct circle_t *lhs, struct circle_t *rhs)
{
    int center_dist;
    int radius_sum;

    center_dist = distance(lhs->x, lhs->y, rhs->x, rhs->y);
    radius_sum = lhs->radius + rhs->radius;
    return radius_sum > center_dist;
}

typedef int shape_cmp_fn(struct shape_t *lhs, struct shape_t *rhs);

static int
shape_intersects_point_point(struct shape_t *lhs, struct shape_t *rhs)
{
    struct point_t *point1;
    struct point_t *point2;

    SHAPE_SELECT(SHAPE_TYPE_POINT, SHAPE_TYPE_POINT, lhs, rhs,
                 &point1, &point2);

    return point_intersects_point(point1, point2);
}

static int
shape_intersects_point_rectangle(struct shape_t *lhs, struct shape_t *rhs)
{
    struct rectangle_t *rectangle;
    struct point_t *point;

    if (lhs->type == SHAPE_TYPE_POINT) {
        assert(rhs->type == SHAPE_TYPE_RECTANGLE);
        point = &lhs->point;
        rectangle = &rhs->rectangle;
    } else {
        assert(lhs->type == SHAPE_TYPE_RECTANGLE);
        assert(rhs->type == SHAPE_TYPE_POINT);
        point = &rhs->point;
        rectangle = &lhs->rectangle;
    }
    return point_intersects_rectangle(point, rectangle);
}

static int
shape_intersects_point_circle(struct shape_t *lhs, struct shape_t *rhs)
{
    struct circle_t *circle;
    struct point_t *point;

    if (lhs->type == SHAPE_TYPE_POINT) {
        assert(rhs->type == SHAPE_TYPE_CIRCLE);
        point = &lhs->point;
        circle = &rhs->circle;
    } else {
        assert(lhs->type == SHAPE_TYPE_CIRCLE);
        assert(rhs->type == SHAPE_TYPE_POINT);
        point = &rhs->point;
        circle = &lhs->circle;
    }
    return point_intersects_circle(point, circle);
}

static int
shape_intersects_rectangle_rectangle(struct shape_t *lhs, struct shape_t *rhs)
{
    assert(lhs->type == SHAPE_TYPE_RECTANGLE);
    assert(rhs->type == SHAPE_TYPE_RECTANGLE);
    return rectangle_intersects_rectangle(&lhs->rectangle, &rhs->rectangle);
}

static int
shape_intersects_rectangle_circle(struct shape_t *lhs, struct shape_t *rhs)
{
    struct rectangle_t *rectangle;
    struct circle_t *circle;

    if (lhs->type == SHAPE_TYPE_RECTANGLE) {
        assert(rhs->type == SHAPE_TYPE_CIRCLE);
        rectangle = &lhs->rectangle;
        circle = &rhs->circle;
    } else {
        assert(lhs->type == SHAPE_TYPE_CIRCLE);
        assert(rhs->type == SHAPE_TYPE_RECTANGLE);
        rectangle = &rhs->rectangle;
        circle = &lhs->circle;
    }
    return rectangle_intersects_circle(rectangle, circle);
}

static int
shape_intersects_circle_circle(struct shape_t *lhs, struct shape_t *rhs)
{
    assert(lhs->type == SHAPE_TYPE_CIRCLE);
    assert(rhs->type == SHAPE_TYPE_CIRCLE);
    return circle_intersects_circle(&lhs->circle, &rhs->circle);
}

static shape_cmp_fn *shape_intersect_cbs[SHAPE_TYPE_MAX][SHAPE_TYPE_MAX] = {
    [SHAPE_TYPE_POINT][SHAPE_TYPE_POINT] =
        shape_intersects_point_point,
    [SHAPE_TYPE_POINT][SHAPE_TYPE_RECTANGLE] =
        shape_intersects_point_rectangle,
    [SHAPE_TYPE_POINT][SHAPE_TYPE_CIRCLE] =
        shape_intersects_point_circle,
    [SHAPE_TYPE_RECTANGLE][SHAPE_TYPE_POINT] =
        shape_intersects_point_rectangle,
    [SHAPE_TYPE_RECTANGLE][SHAPE_TYPE_RECTANGLE] =
        shape_intersects_rectangle_rectangle,
    [SHAPE_TYPE_RECTANGLE][SHAPE_TYPE_CIRCLE] =
        shape_intersects_rectangle_circle,
    [SHAPE_TYPE_CIRCLE][SHAPE_TYPE_POINT] =
        shape_intersects_point_circle,
    [SHAPE_TYPE_CIRCLE][SHAPE_TYPE_RECTANGLE] =
        shape_intersects_rectangle_circle,
    [SHAPE_TYPE_CIRCLE][SHAPE_TYPE_CIRCLE] =
        shape_intersects_circle_circle,
};

int
shape_intersects(struct shape_t *lhs, struct shape_t *rhs)
{
    shape_cmp_fn *cb;
    int rc;

    cb = shape_intersect_cbs[lhs->type][rhs->type];
    rc = cb(lhs, rhs);
    return rc;
}

/**
 * @return                      0 if there is an intersection; else nonzero.
 */
static int
rect_intersection_raw(int r1x1, int r1y1, int r1w, int r1h,
                      int r2x1, int r2y1, int r2w, int r2h,
                      int *x1, int *y1, int *w, int *h)
{
    int r1x2;
    int r1y2;
    int r2x2;
    int r2y2;
    int x2;
    int y2;

    r1x2 = r1x1 + r1w;
    r1y2 = r1y1 + r1h;
    r2x2 = r2x1 + r2w;
    r2y2 = r2y1 + r2h;

    *x1 = r1x1 > r2x1 ? r1x1 : r2x1;
    *y1 = r1y1 > r2y1 ? r1y1 : r2y1;

    x2 = r1x2 < r2x2 ? r1x2 : r2x2;
    y2 = r1y2 < r2y2 ? r1y2 : r2y2;
    *w = x2 - *x1;
    *h = y2 - *y1;

    if (*w > 0 && *h > 0) {
        return 0;
    } else {
        return -1;
    }
}

/**
 * @return                      0 if there is an intersection; else nonzero.
 */
int
rect_intersection(struct rectangle_t *in1, struct rectangle_t *in2,
                  struct rectangle_t *out)
{
    return rect_intersection_raw(in1->x, in1->y, in1->dimx, in1->dimy,
                                 in2->x, in2->y, in2->dimx, in2->dimy,
                                 &out->x, &out->y, &out->dimx, &out->dimy);
}

/**
 * @return                      0 if there is an intersection; else nonzero.
 */
static int
rectangle_intersection_center_rectangle_raw(int r1x1, int r1y1, int r1w, int r1h,
                             int r2x1, int r2y1, int r2w, int r2h,
                             int *centerx, int *centery)
{
    int rc;
    int x1;
    int y1;
    int w;
    int h;

    rc = rect_intersection_raw(r1x1, r1y1, r1w, r1h, r2x1, r2y1, r2w, r2h,
                               &x1, &y1, &w, &h);
    if (rc != 0) {
        return rc;
    }

    *centerx = x1 + w / 2;
    *centery = y1 + h / 2;

    return 0;
}

/**
 * @return                      0 if there is an intersection; else nonzero.
 */
static int
point_intersection_center_point(struct point_t *lhs, struct point_t *rhs,
                                struct point_t *out_center)
{
    if (point_intersects_point(lhs, rhs)) {
        *out_center = *lhs;
        return 0;
    }

    return -1;
}

/**
 * @return                      0 if there is an intersection; else nonzero.
 */
static int
point_intersection_center_rectangle(struct point_t *point,
                                    struct rectangle_t *rectangle,
                                    struct point_t *out_center)
{
    if (point_intersects_rectangle(point, rectangle)) {
        *out_center = *point;
        return 0;
    }

    return -1;
}

/**
 * @return                      0 if there is an intersection; else nonzero.
 */
static int
point_intersection_center_circle(struct point_t *point,
                                 struct circle_t *circle,
                                 struct point_t *out_center)
{
    if (point_intersects_circle(point, circle)) {
        *out_center = *point;
        return 0;
    }

    return -1;
}

/**
 * @return                      0 if there is an intersection; else nonzero.
 */
static int
rectangle_intersection_center_rectangle(struct rectangle_t *lhs,
                                        struct rectangle_t *rhs,
                                        struct point_t *out_center)
{
    return rectangle_intersection_center_rectangle_raw(lhs->x, lhs->y,
                                                       lhs->dimx, lhs->dimy,
                                                       rhs->x, rhs->y,
                                                       rhs->dimx, rhs->dimy,
                                                       &out_center->x,
                                                       &out_center->y);
}

static int
shape_intersection_center_point_point(struct shape_t *lhs, struct shape_t *rhs,
                                      struct point_t *out_center)
{
    struct point_t *point1;
    struct point_t *point2;

    SHAPE_SELECT(SHAPE_TYPE_POINT, SHAPE_TYPE_POINT, lhs, rhs,
                 &point1, &point2);

    return point_intersection_center_point(point1, point2, out_center);
}

static int
shape_intersection_center_point_rectangle(struct shape_t *lhs,
                                          struct shape_t *rhs,
                                          struct point_t *out_center)
{
    struct rectangle_t *rectangle;
    struct point_t *point;

    if (lhs->type == SHAPE_TYPE_POINT) {
        assert(rhs->type == SHAPE_TYPE_RECTANGLE);
        point = &lhs->point;
        rectangle = &rhs->rectangle;
    } else {
        assert(lhs->type == SHAPE_TYPE_RECTANGLE);
        assert(rhs->type == SHAPE_TYPE_POINT);
        point = &rhs->point;
        rectangle = &lhs->rectangle;
    }
    return point_intersection_center_rectangle(point, rectangle, out_center);
}

static int
shape_intersection_center_point_circle(struct shape_t *lhs,
                                       struct shape_t *rhs,
                                       struct point_t *out_center)
{
    struct circle_t *circle;
    struct point_t *point;

    if (lhs->type == SHAPE_TYPE_POINT) {
        assert(rhs->type == SHAPE_TYPE_CIRCLE);
        point = &lhs->point;
        circle = &rhs->circle;
    } else {
        assert(lhs->type == SHAPE_TYPE_CIRCLE);
        assert(rhs->type == SHAPE_TYPE_POINT);
        point = &rhs->point;
        circle = &lhs->circle;
    }
    return point_intersection_center_circle(point, circle, out_center);
}

static int
shape_intersection_center_rectangle_rectangle(struct shape_t *lhs,
                                              struct shape_t *rhs,
                                              struct point_t *out_center)
{
    struct rectangle_t *rectangle1;
    struct rectangle_t *rectangle2;

    SHAPE_SELECT(SHAPE_TYPE_RECTANGLE, SHAPE_TYPE_RECTANGLE, lhs, rhs,
                 &rectangle1, &rectangle2);

    return rectangle_intersection_center_rectangle(rectangle1, rectangle2,
                                                   out_center);
}

typedef int shape_cmp_point_fn(struct shape_t *lhs, struct shape_t *rhs,
                               struct point_t *out_center);

static shape_cmp_point_fn *
shape_intersection_center_cbs[SHAPE_TYPE_MAX][SHAPE_TYPE_MAX] = {
    [SHAPE_TYPE_POINT][SHAPE_TYPE_POINT] =
        shape_intersection_center_point_point,
    [SHAPE_TYPE_POINT][SHAPE_TYPE_RECTANGLE] =
        shape_intersection_center_point_rectangle,
    [SHAPE_TYPE_POINT][SHAPE_TYPE_CIRCLE] =
        shape_intersection_center_point_circle,
    [SHAPE_TYPE_RECTANGLE][SHAPE_TYPE_POINT] =
        shape_intersection_center_point_rectangle,
    [SHAPE_TYPE_RECTANGLE][SHAPE_TYPE_RECTANGLE] =
        shape_intersection_center_rectangle_rectangle,
    [SHAPE_TYPE_RECTANGLE][SHAPE_TYPE_CIRCLE] =
        NULL,
    [SHAPE_TYPE_CIRCLE][SHAPE_TYPE_POINT] =
        shape_intersection_center_point_circle,
    [SHAPE_TYPE_CIRCLE][SHAPE_TYPE_RECTANGLE] =
        NULL,
    [SHAPE_TYPE_CIRCLE][SHAPE_TYPE_CIRCLE] =
        NULL,
};

int
shape_intersection_center(struct shape_t *lhs, struct shape_t *rhs,
                          struct point_t *out_center)
{
    shape_cmp_point_fn *cb;
    int rc;

    cb = shape_intersection_center_cbs[lhs->type][rhs->type];
    assert(cb != NULL);
    rc = cb(lhs, rhs, out_center);
    return rc;
}

int
seg_intersects_box(int sx1, int sy1, int sx2, int sy2,
                   int bx1, int by1, int bx2, int by2)
{
    double tmp;
    double a;
    double b;
    int minx;
    int maxx;
    int miny;
    int maxy;
    int dx;

    // Find min and max X for the segment
    if (sx1 < sx2) {
        minx = sx1;
        maxx = sx2;
    } else {
        minx = sx2;
        maxx = sx1;
    }

    // Find the intersection of the segment's and rectangle's x-projections
    if (maxx > bx2) {
        maxx = bx2;
    }

    if (minx < bx1) {
        minx = bx1;
    }

    if (minx > maxx) {
        return 0;
    }

    // Find corresponding min and max Y for min and max X we found before

    miny = sy1;
    maxy = sy2;

    dx = sx2 - sx1;

    if (dx > 0) {
        a = (sy2 - sy1) / dx;
        b = sy1 - a * sx1;
        miny = a * minx + b;
        maxy = a * maxx + b;
    }

    if (miny > maxy) {
        tmp = maxy;
        maxy = miny;
        miny = tmp;
    }

    // Find the intersection of the segment's and rectangle's y-projections
    if (maxy > by2) {
        maxy = by2;
    }

    if (miny < by1) {
        miny = by1;
    }

    // If Y-projections do not intersect return false
    if (miny > maxy) {
        return 0;
    }

    return 1;
}

int
segment_tile_foreach(int x1, int y1, int x2, int y2,
                     int tile_dimx, int tile_dimy,
                     segment_tile_foreach_cb_t cb, void *arg)
{
    double slope;
    int nextedgex;
    int nextedgey;
    int tilexend;
    int tileyend;
    int toedgex;
    int toedgey;
    int deltax;
    int deltay;
    int signx;
    int signy;
    int tilex;
    int tiley;
    int timex;
    int timey;
    int rc;
    int x;
    int y;

    assert(x1 >= 0 && x2 >= 0 && y1 >= 0 && y2 >= 0);

    rc = cb(x1 / tile_dimx, y1 / tile_dimy, arg);
    if (rc != 0) {
        return rc;
    }

    tilexend = x2 / tile_dimx;
    tileyend = y2 / tile_dimy;

    deltax = x2 - x1;
    deltay = y2 - y1;
    if (deltax == 0) {
        slope = DBL_MAX;
    } else {
        slope = (double)deltay / (double)deltax;
    }

    signx = (x2 > x1) - (x2 < x1);
    signy = (y2 > y1) - (y2 < y1);

    x = x1;
    y = y1;
    while (1) {
        tilex = x / tile_dimx;
        tiley = y / tile_dimy;

        if ((tilex == tilexend && tiley == tileyend)    ||
            (signx == -1    && tilex < tilexend)        ||
            (signx == 1     && tilex > tilexend)        ||
            (signy == -1    && tiley < tileyend)        ||
            (signy == 1     && tiley > tileyend)) {

            return 0;
        }

        rc = cb(tilex, tiley, arg);
        if (rc != 0) {
            return rc;
        }

        if (signx == -1) {
            nextedgex = (x - 1) / tile_dimx * tile_dimx - 1;
            toedgex = x - nextedgex;
        } else {
            nextedgex = (x / tile_dimx + 1) * tile_dimx;
            toedgex = nextedgex - x;
        }
        if (signy == -1) {
            nextedgey = (y - 1) / tile_dimy * tile_dimy - 1;
            toedgey = y - nextedgey;
        } else {
            nextedgey = (y / tile_dimy + 1) * tile_dimy;
            toedgey = nextedgey - y;
        }

        if (signx == 0) {
            timex = INT_MAX;
        } else {
            timex = toedgex;
        }
        if (signy == 0) {
            timey = INT_MAX;
        } else {
            timey = toedgey / fabs(slope);
        }

        if (timex < timey) {
            x = nextedgex;
            y += signy * fabs(timex * slope);
        } else {
            assert(slope != 0.0);
            x += signx * timey;
            y = nextedgey;
        }
    }
}

struct segment_tile_intersections_arg_t {
    int (*intersections)[2];
    int num_intersections;
    int max_intersections;
};

static int
segment_tile_intersections_cb(int tilex, int tiley, void *arg)
{
    struct segment_tile_intersections_arg_t *cb_arg;

    cb_arg = arg;
    if (cb_arg->num_intersections >= cb_arg->max_intersections) {
        return -1;
    }

    cb_arg->intersections[cb_arg->num_intersections][0] = tilex;
    cb_arg->intersections[cb_arg->num_intersections][1] = tiley;
    cb_arg->num_intersections++;

    return 0;
}

int
segment_tile_intersections(int x1, int y1, int x2, int y2,
                           int tile_dimx, int tile_dimy,
                           int (*intersections)[2], int *num_intersections)
{
    struct segment_tile_intersections_arg_t arg;
    int rc;

    arg.intersections = intersections;
    arg.num_intersections = 0;
    arg.max_intersections = *num_intersections;

    rc = segment_tile_foreach(x1, y1, x2, y2, tile_dimx, tile_dimy,
                              segment_tile_intersections_cb, &arg);
    *num_intersections = arg.num_intersections;
    return rc;
}

void
line_from_seg(struct line_t *line, int x1, int y1, int x2, int y2)
{
    assert(x1 != x2 || y1 != y2);

    if (x1 == x2) {
        /* For vertical lines, slope is +/- infinity, and offset is the
         * *x-intercept*.
         */
        if (y1 > y2) {
            /* Line goes up. */
            line->slope = INFINITY;
        } else {
            line->slope = -INFINITY;
        }
        line->offset = x1;
    } else {
        /* Slope is "rise over run."  Y values are reversed to account for
         * opposite polarities of screen coordinate vs. cartesian coordinates.
         */
        line->slope = (double)(y1 - y2) / (x2 - x1);

        /* Solve for b via: y = mx + b                      <==>
         *                  b = y - mx                      <==>
         *                  b = -y - mx [y rev. polarity; slope already good]
         */
        line->offset = -y1 - line->slope * x1;
    }
}

void
line_from_point_angle(struct line_t *line, int x, int y, int degrees)
{
    int x2;
    int y2;

    angle_to_vect(degrees, 100000, &x2, &y2);
    x2 += x;
    y2 -= y;

    line_from_seg(line, x, y, x2, y2);
}

int
line_x_given_y(struct line_t *line, int y)
{
    if (isinf(line->slope)) {
        return line->offset;
    }

    /* y = mx + b */
    /* x = (y - b) / m */
    /* x = (y - b) / -slope  (reverse polarity). */
    return (y + line->offset) / -line->slope;
}

int
line_y_given_x(struct line_t *line, int x)
{
    assert(!isinf(line->slope));

    /* y = mx + b */
    /* y = (-slope)x + b  (reverse polarity). */
    return -line->slope * x - line->offset;
}

static int
point_set_add(int (*points)[2], int num_points, int x, int y, int x2, int y2)
{
    int i;

    for (i = 0; i < num_points; i++) {
        /* Don't allow the same point to be added twice. */
        if (points[i][0] == x && points[i][1] == y) {
            return num_points;
        }

        /* Don't allow two points on the same edge. */
        if (x == 0 && points[i][0] == 0) {
            return num_points;
        }
        if (x == x2 && points[i][0] == x2) {
            return num_points;
        }
        if (y == 0 && points[i][1] == 0) {
            return num_points;
        }
        if (y == y2 && points[i][1] == y2) {
            return num_points;
        }
    }

    points[i][0] = x;
    points[i][1] = y;

    return i + 1;
}

int
line_rect_intersection_points(struct line_t *line,
                              int rx1, int ry1, int rdimx, int rdimy,
                              int (*points)[2])
{
    int num_points;
    int inf;
    int rx2;
    int ry2;
    int x;
    int y;
    int i;

    rx2 = rx1 + rdimx - 1;
    ry2 = ry1 + rdimy - 1;

    inf = isinf(line->slope);
    if (inf != 0) {
        if (line->offset >= rx1 && line->offset < rx2) {
            points[0][0] = line->offset;
            points[0][1] = inf == 1 ? ry1 : ry2;
            points[1][0] = line->offset;
            points[1][1] = inf == 1 ? ry2 : ry1;
            return 2;
        } else {
            return 0;
        }
    }

    num_points = 0;

    for (i = 0; i < 4; i++) {
        switch (i) {
        case 0:
            /* Top. */
            x = line_x_given_y(line, ry1);
            if (x >= rx1 && x <= rx2) {
                num_points = point_set_add(points, num_points, x, ry1,
                                           rx2, ry2);
            }
            break;

        case 1:
            /* Right. */
            y = line_y_given_x(line, rx2);
            if (y >= ry1 && y <= ry2) {
                num_points = point_set_add(points, num_points, rx2, y,
                                           rx2, ry2);
            }
            break;

        case 2:
            /* Bottom. */
            x = line_x_given_y(line, ry2);
            if (x >= rx1 && x <= rx2) {
                num_points = point_set_add(points, num_points, x, ry2,
                                           rx2, ry2);
            }
            break;

        case 3:
            /* Left. */
            y = line_y_given_x(line, rx1);
            if (y >= ry1 && y <= ry2) {
                num_points = point_set_add(points, num_points, rx1, y,
                                           rx2, ry2);
            }
            break;

        default:
            assert(0);
            break;
        }

        assert(num_points <= 2);
        if (num_points == 2) {
            return 2;
        }
    }

    return num_points;
}

/**
 * Takes as input a line which intersects a rectangle.  If the line intersects
 * the rectangle at an edge or a corner, this function moves the line closer
 * to the center of the rectangle, so that the two intersection points are
 * at least 'margin' units from each of the rectangle's four corners.  This
 * function does not modify the line if the intersection points are already
 * inside the margin.
 */
void
line_rect_ensure_margin(struct line_t *line, int rx1, int ry1,
                        int rdimx, int rdimy, int margin)
{
    
    int x_at_ry1;
    int x_at_ry2;
    int y_at_rx1;
    int y_at_rx2;
    int minx;
    int maxx;
    int miny;
    int maxy;
    int rx2;
    int ry2;

    rx2 = rx1 + rdimx - 1;
    ry2 = ry1 + rdimy - 1;

    minx = rx1 + margin;
    maxx = rx2 - margin;
    miny = ry1 + margin;
    maxy = ry2 - margin;

    if (isinf(line->slope)) {
        if (line->offset < minx) {
            line->offset = minx;
        } else if (line->offset > maxx) {
            line->offset = maxx;
        }
        return;
    }

    y_at_rx1 = line_y_given_x(line, rx1);
    y_at_rx2 = line_y_given_x(line, rx2);

    if        (y_at_rx1 >= ry1 && y_at_rx1 < miny && y_at_rx2 < miny) {
        line->offset -= miny - y_at_rx1;
    } else if (y_at_rx2 >= ry1 && y_at_rx2 < miny && y_at_rx1 < miny) {
        line->offset -= miny - y_at_rx2;
    }

    if        (y_at_rx1 <= ry2 && y_at_rx1 > maxy && y_at_rx2 > maxy) {
        line->offset += maxy - y_at_rx1;
    } else if (y_at_rx2 <= ry2 && y_at_rx2 > maxy && y_at_rx1 > maxy) {
        line->offset += maxy - y_at_rx2;
    }

    x_at_ry1 = line_x_given_y(line, ry1);
    x_at_ry2 = line_x_given_y(line, ry2);

    if        (x_at_ry1 >= rx1 && x_at_ry1 < minx && x_at_ry2 < minx) {
        line->offset -= ceil(line->slope * (minx - x_at_ry1));
    } else if (x_at_ry2 >= rx1 && x_at_ry2 < minx && x_at_ry1 < minx) {
        line->offset -= ceil(line->slope * (minx - x_at_ry2));
    }

    if        (x_at_ry1 <= rx2 && x_at_ry1 > maxx && x_at_ry2 > maxx) {
        /* Negative slope; move line down. */
        line->offset += ceil(line->slope * (maxx - x_at_ry1));
    } else if (x_at_ry2 <= rx2 && x_at_ry2 > maxx && x_at_ry1 > maxx) {
        /* Positive slope; move line up. */
        line->offset += ceil(line->slope * (maxx - x_at_ry2));
    }
}

int
degrees_from_slope(double m)
{
    struct line_t line;
    int inf;
    int x;
    int y;

    inf = isinf(m);
    if (inf == 1) {
        return 90;
    } else if (inf == -1) {
        return 270;
    } else {
        line.slope = m;
        line.offset = 0;
        if (m >= 0.0) {
            x = 100000;
        } else {
            x = -100000;
        }
        y = line_y_given_x(&line, x); 
        return angle_from_vect(x, y);
    }
}

void
calc_chunks(struct calc_chunks_req_t *req, struct calc_chunks_rsp_t *rsp)
{
    int chunkx2;
    int chunky2;
    int x2;
    int y2;

    assert(req->dimx > 0);
    assert(req->dimy > 0);
    assert(req->chunk_dimx > 0);
    assert(req->chunk_dimy > 0);

    x2 = req->x + req->dimx - 1;
    rsp->chunkx = div_floor(req->x, req->chunk_dimx);
    chunkx2 = div_floor(x2, req->chunk_dimx);
    rsp->sub_offx = rsp->chunkx * req->chunk_dimx - req->x;
    rsp->num_chunksx = chunkx2 - rsp->chunkx + 1;

    y2 = req->y + req->dimy - 1;
    rsp->chunky = div_floor(req->y, req->chunk_dimy);
    chunky2 = div_floor(y2, req->chunk_dimy);
    rsp->sub_offy = (rsp->chunky * req->chunk_dimy - req->y);
    rsp->num_chunksy = chunky2 - rsp->chunky + 1;
}

#ifndef H_GEN_SHAPE_
#define H_GEN_SHAPE_

#define SHAPE_TYPE_POINT            0
#define SHAPE_TYPE_RECTANGLE        1
#define SHAPE_TYPE_CIRCLE           2
#define SHAPE_TYPE_MAX              3

struct point_t {
    int x;
    int y;
};

struct rectangle_t {
    int x;
    int y;
    int dimx;
    int dimy;
};

struct circle_t {
    int x;
    int y;
    int radius;
};

struct shape_t {
    union {
        struct point_t point;
        struct rectangle_t rectangle;
        struct circle_t circle;
    };
    int type;
};

struct line_t {
    /**
     * (DeltaY / DeltaX).
     * NOTE: Greater Y <==> higher.
     */
    double slope;

    /**
     * X-intercept for vertical lines;
     * Y-intercept in all other cases (positive = above X-axis).
     */
    int offset;
};

int point_intersects_point(struct point_t *lhs, struct point_t *rhs);
int point_intersects_rectangle(struct point_t *point,
                               struct rectangle_t *rect);
int point_intersects_circle(struct point_t *point, struct circle_t *circle);
int rectangle_intersects_rectangle(struct rectangle_t *r1,
                                   struct rectangle_t *r2);
int rectangle_intersects_circle(struct rectangle_t *rectangle,
                                struct circle_t *circle);
int circle_intersects_circle(struct circle_t *lhs, struct circle_t *rhs);
int shape_intersects(struct shape_t *lhs, struct shape_t *rhs);
int shape_intersection_center(struct shape_t *lhs, struct shape_t *rhs,
                              struct point_t *out_center);
int rect_intersection(struct rectangle_t *in1, struct rectangle_t *in2,
                      struct rectangle_t *out);
int traj_intersects_box(int originx, int originy, int speedx, int speedy,
                        int x1, int y1, int width, int height);
int seg_intersects_box(int sx1, int sy1, int sx2, int sy2,
                       int bx1, int by1, int bx2, int by2);

typedef int segment_tile_foreach_cb_t(int tilex, int tiley, void *arg);
int segment_tile_foreach(int x1, int y1, int x2, int y2,
                         int tile_dimx, int tile_dimy,
                         segment_tile_foreach_cb_t cb, void *arg);
int segment_tile_intersections(int x1, int y1, int x2, int y2,
                               int tile_dimx, int tile_dimy,
                               int (*intersections)[2],
                               int *num_intersections);
void line_from_seg(struct line_t *line, int x1, int y1, int x2, int y2);
void line_from_point_angle(struct line_t *line, int x, int y, double rads);
int line_x_given_y(struct line_t *line, int y);
int line_y_given_x(struct line_t *line, int x);
int line_rect_intersection_points(struct line_t *line,
                                  int rx1, int ry1, int rdimx, int rdimy,
                                  int (*points)[2]);

double rads_from_slope(double m);

void line_rect_ensure_margin(struct line_t *line,
                                    int rx1, int ry1, int rdimx, int rdimy,
                                    int margin);


struct calc_chunks_req_t {
    int x;
    int y;
    int dimx;
    int dimy;
    int chunk_dimx;
    int chunk_dimy;
};

struct calc_chunks_rsp_t {
    int chunkx;
    int chunky;
    int sub_offx;
    int sub_offy;
    int num_chunksx;
    int num_chunksy;
};

void
calc_chunks(struct calc_chunks_req_t *req, struct calc_chunks_rsp_t *rsp);

#endif


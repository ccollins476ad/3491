#ifndef WORLD_H_
#define WORLD_H_

#include <inttypes.h>
#include "gen/gen_shape.h"
#include "gfx/tile.h"
#include "gfx/canvas.h"
struct color_info_t;
struct calc_chunks_req_t;
struct calc_chunks_rsp_t;

#define WORLD_FACTOR    256
#define WORLD_TILE_DIM  (TILE_DIM * WORLD_FACTOR)

#define WORLD_ZONE_BASE_DIMX        (640 * WORLD_FACTOR)
#define WORLD_ZONE_BASE_DIMY        (480 * WORLD_FACTOR)
#define WORLD_ZONE_BEING_MARGIN     (320 * WORLD_FACTOR)
#define WORLD_ZONE_BULLET_MARGIN    (320 * WORLD_FACTOR)

#define WORLD_ZONE_BEING_DIMX       \
    (WORLD_ZONE_BASE_DIMX + WORLD_ZONE_BEING_MARGIN * 2)
#define WORLD_ZONE_BEING_DIMY       \
    (WORLD_ZONE_BASE_DIMY + WORLD_ZONE_BEING_MARGIN * 2)

#define WORLD_ZONE_BULLET_DIMX      \
    (WORLD_ZONE_BASE_DIMX + WORLD_ZONE_BULLET_MARGIN * 2)
#define WORLD_ZONE_BULLET_DIMY      \
    (WORLD_ZONE_BASE_DIMY + WORLD_ZONE_BULLET_MARGIN * 2)

struct BITMAP;
struct terr_scape_t;
struct hit_box_t;
struct hit_box_desc_t;
struct icon_t;

/** @world@ */
int world_scape_width(void);
int world_scape_height(void);
void world_fill_zone(void);
int world_segment_tile_foreach(int x1, int y1, int x2, int y2,
                               segment_tile_foreach_cb_t cb, void *arg);

extern unsigned int world_tick;


/** @world_view@ */
struct world_view_info_t {
    struct canvas_t *canvas;
    int worldx;
    int worldy;
    int world_dimx;
    int world_dimy;
};

void
world_view_chunks(int chunk_dimx, int chunk_dimy,
                  struct calc_chunks_rsp_t *rsp);

void world_view_canvas_loc(int x, int y, int *outx, int *outy);
void world_view_tile_loc(int worldx, int worldy,
                         int *out_tilex, int *out_tiley);
void world_view_loc_snap_to_tile(int worldx, int worldy, int *out_worldx,
                                 int *out_worldy);
void world_view_draw_point(int x, int y, uint32_t color);
void world_view_draw_rect(int x, int y, int w, int h, uint32_t color);
void world_view_draw_circle(int x, int y, int r, uint32_t color);
void world_view_draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void world_view_draw_icon(struct icon_t *icon, struct color_info_t *color_info,
                          int worldx, int worldy);
void world_view_smart_draw(struct BITMAP *src, int x, int y,
                           struct color_info_t *color_info,
                           double scale, double angle, int hflip, int vflip);
void world_view_set_center(int world_centerx, int world_centery);
void world_view_set_canvas(struct canvas_t *canvas);

extern struct world_view_info_t world_view_info;

#endif


#include "gen/gen_num.h"
#include "gen/gen_shape.h"
#include "gen/gen_dbg.h"
#include "gfx/balleg.h"
#include "gfx/gfx.h"
#include "gfx/canvas.h"
#include "gfx/icon.h"
#include "gfx/tile.h"
#include "life/world.h"
#include "life/being.h"

struct world_view_info_t world_view_info;

void
world_view_set_center(int world_centerx, int world_centery)
{
    world_view_info.worldx = world_centerx - world_view_info.world_dimx / 2;
    world_view_info.worldy = world_centery - world_view_info.world_dimy / 2;
}

void
world_view_chunks(int chunk_dimx, int chunk_dimy,
                  struct calc_chunks_rsp_t *rsp)
{
    struct calc_chunks_req_t req;

    req.x = world_view_info.worldx;
    req.y = world_view_info.worldy;
    req.dimx = world_view_info.world_dimx;
    req.dimy = world_view_info.world_dimy;
    req.chunk_dimx = chunk_dimx;
    req.chunk_dimy = chunk_dimy;

    calc_chunks(&req, rsp);

    /* Convert tile offsets from world coordinates to pixel coordinates. */
    rsp->sub_offx = div_floor(rsp->sub_offx, WORLD_FACTOR);
    rsp->sub_offy = div_floor(rsp->sub_offy, WORLD_FACTOR);

    if (rsp->sub_offx == -chunk_dimx / WORLD_FACTOR) {
        rsp->chunkx++;
        rsp->num_chunksx--;
        rsp->sub_offx = 0;
    }
    if (rsp->sub_offy == -chunk_dimy / WORLD_FACTOR) {
        rsp->chunky++;
        rsp->num_chunksy--;
        rsp->sub_offy = 0;
    }
}

void
world_view_canvas_loc(int x, int y, int *outx, int *outy)
{
    *outx = div_floor(x - world_view_info.worldx, WORLD_FACTOR);
    *outy = div_floor(y - world_view_info.worldy, WORLD_FACTOR);
}

void
world_view_tile_loc(int worldx, int worldy, int *out_tilex, int *out_tiley)
{
    *out_tilex = div_floor(worldx, WORLD_TILE_DIM);
    *out_tiley = div_floor(worldy, WORLD_TILE_DIM);
}

void
world_view_loc_snap_to_tile(int worldx, int worldy,
                            int *out_worldx, int *out_worldy)
{
    int tilex;
    int tiley;

    world_view_tile_loc(worldx, worldy, &tilex, &tiley);
    *out_worldx = tilex * WORLD_TILE_DIM;
    *out_worldy = tiley * WORLD_TILE_DIM;
}

void
world_view_draw_point(int x, int y, uint32_t color)
{
    int canvasx;
    int canvasy;

    world_view_canvas_loc(x, y, &canvasx, &canvasy);
    putpixel(world_view_info.canvas->bmp, canvasx, canvasy, color);
}

void
world_view_draw_rect(int x, int y, int w, int h, uint32_t color)
{
    int canvasx1;
    int canvasy1;
    int canvasx2;
    int canvasy2;
    int screenx1;
    int screeny1;
    int screenx2;
    int screeny2;

    world_view_canvas_loc(x, y, &canvasx1, &canvasy1);
    world_view_canvas_loc(x + w - 1, y + h - 1, &canvasx2, &canvasy2);
    screenx1 = canvasx1 + world_view_info.canvas->x;
    screeny1 = canvasy1 + world_view_info.canvas->y;
    screenx2 = canvasx2 + world_view_info.canvas->x;
    screeny2 = canvasy2 + world_view_info.canvas->y;

    rect(world_view_info.canvas->bmp, screenx1, screeny1, screenx2, screeny2,
         color);
}

void
world_view_draw_circle(int x, int y, int r, uint32_t color)
{
    int canvasx;
    int canvasy;
    int canvasr;

    world_view_canvas_loc(x, y, &canvasx, &canvasy);
    canvasr = r / WORLD_FACTOR;

    circle(world_view_info.canvas->bmp, canvasx, canvasy, canvasr, color);
}

void
world_view_draw_line(int x1, int y1, int x2, int y2, uint32_t color)
{
    int canvasx1;
    int canvasy1;
    int canvasx2;
    int canvasy2;

    world_view_canvas_loc(x1, y1, &canvasx1, &canvasy1);
    world_view_canvas_loc(x2, y2, &canvasx2, &canvasy2);

    line(world_view_info.canvas->bmp, canvasx1, canvasy1, canvasx2, canvasy2,
         color);
}

void
world_view_draw_icon(struct icon_t *icon, struct color_info_t *color_info,
                     int x, int y)
{
    int canvasx;
    int canvasy;

    world_view_canvas_loc(x, y, &canvasx, &canvasy);
    tile_draw(world_view_info.canvas, canvasx, canvasy,
              icon->tile_id, icon->fg, color_info);
}

void
world_view_smart_draw(struct BITMAP *src, int x, int y,
                      struct color_info_t *color_info,
                      double scale, double angle, int hflip, int vflip)
{
    int canvasx;
    int canvasy;
    int bmpx;
    int bmpy;

    world_view_canvas_loc(x, y, &canvasx, &canvasy);
    bmpx = canvasx + world_view_info.canvas->x;
    bmpy = canvasy + world_view_info.canvas->y;

    gfx_smart_draw(world_view_info.canvas->bmp, src, bmpx, bmpy,
                   color_info, scale, angle, hflip, vflip);
}

void
world_view_set_canvas(struct canvas_t *canvas)
{
    world_view_info.canvas = canvas;
    world_view_info.world_dimx = canvas->dimx * WORLD_FACTOR;
    world_view_info.world_dimy = canvas->dimy * WORLD_FACTOR;
}


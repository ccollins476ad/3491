#ifndef H_TILE_
#define H_TILE_

#include <inttypes.h>
struct BITMAP;
struct color_info_t;
struct canvas_t;

#define TILE_MAX        256
#define TILE_DIM        8

#define TILE_ID_NONE    0
#define TILE_ID_FACE_HOLLOW 1
#define TILE_ID_FACE_SOLID  2
#define TILE_ID_BLOCK_25    176 /* 25% */
#define TILE_ID_BLOCK_50    177 /* 50% */
#define TILE_ID_BLOCK_75    178 /* 75% */
#define TILE_ID_BLOCK_100   219 /* 100% */
#define TILE_ID_BLOCK_SMALL 254
#define TILE_ID_BAR_V       179
#define TILE_ID_BAR_H       196
#define TILE_ID_STAIRS      240

#define TILE_ID_BORDER_SINGLE_UL    218
#define TILE_ID_BORDER_SINGLE_UR    191
#define TILE_ID_BORDER_SINGLE_DR    217
#define TILE_ID_BORDER_SINGLE_DL    192
#define TILE_ID_BORDER_SINGLE_H     TILE_ID_BAR_H
#define TILE_ID_BORDER_SINGLE_V     TILE_ID_BAR_V

typedef uint8_t tile_id_t;

void tile_sprite_clear(void);
struct BITMAP *tile_to_bitmap(int tile_id, uint32_t color);

void
tile_draw_bg_raw(struct BITMAP *bmp, int bmpx, int bmpy, tile_id_t tile_id,
                 uint32_t color, uint32_t bg);
void
tile_draw_bg(struct canvas_t *canvas, int canvasx, int canvasy,
             tile_id_t tile_id, uint32_t color, uint32_t bg);
void
tile_draw_no_alpha_raw(struct BITMAP *bmp, int bmpx, int bmpy,
                       tile_id_t tile_id, uint32_t color);
void tile_draw_no_alpha(struct canvas_t *canvas, int canvasx, int canvasy,
                        tile_id_t tile_id, uint32_t color);
void tile_draw(struct canvas_t *canvas, int canvasx, int canvasy,
               tile_id_t tile_id, uint32_t color,
               struct color_info_t *color_info);

void tile_init(void);

#endif

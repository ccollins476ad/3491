#include <assert.h>
#include "gen/container.h"
#include "gen/res.h"
#include "gfx/balleg.h"
#include "gfx/canvas.h"
#include "gfx/gfx.h"
#include "gfx/tile.h"

#define TILE_SPRITE_HASH_SZ     1024
#define TILE_SPRITE_MAX_ENTRIES 1024

struct tile_sprite_entry_t {
    struct tile_sprite_entry_t *next;
    struct BITMAP *bmp;
    tile_id_t tile_id;
    uint32_t color;
};

static struct {
    struct BITMAP *tiles[TILE_MAX];
    struct tile_sprite_entry_t *sprite_hash[TILE_SPRITE_HASH_SZ];
    struct pslab_t sprite_pslab;
} tile_data;

static int
tile_sprite_hash_fn(tile_id_t tile_id, uint32_t color)
{
    uint32_t val;

    assert((color & 0xff000000) == 0);

    val = tile_id + (color << 8);

    return val % TILE_SPRITE_HASH_SZ;
}

static struct tile_sprite_entry_t *
tile_sprite_lookup(tile_id_t tile_id, uint32_t color)
{
    struct tile_sprite_entry_t *entry;
    struct tile_sprite_entry_t *prev;
    int hash_val;

    /* Clear alpha channel. */
    color &= 0x00ffffff;

    hash_val = tile_sprite_hash_fn(tile_id, color);

    prev = NULL;
    for (entry = tile_data.sprite_hash[hash_val];
         entry != NULL;
         entry = entry->next) {

        if (entry->tile_id == tile_id && entry->color == color) {
            /* Move requested entry to the front of the list. */
            if (prev != NULL) {
                prev->next = entry->next;
                entry->next = tile_data.sprite_hash[hash_val];
                tile_data.sprite_hash[hash_val] = entry;
            }
            return entry;
        }

        prev = entry;
    }

    return NULL;
}

static struct tile_sprite_entry_t *
tile_sprite_add(tile_id_t tile_id, uint32_t color)
{
    struct tile_sprite_entry_t *entry;
    int hash_val;

    entry = pslab_insert(&tile_data.sprite_pslab);
    if (entry == NULL) {
        return NULL;
    }

    /* Clear alpha channel. */
    color &= 0x00ffffff;

    hash_val = tile_sprite_hash_fn(tile_id, color);
    entry->tile_id = tile_id;
    entry->color = color;
    entry->bmp = tile_to_bitmap(tile_id, color);

    entry->next = tile_data.sprite_hash[hash_val];
    tile_data.sprite_hash[hash_val] = entry;

    return entry;
}

static struct tile_sprite_entry_t *
tile_sprite_ensure(tile_id_t tile_id, uint32_t color)
{
    struct tile_sprite_entry_t *entry;

    entry = tile_sprite_lookup(tile_id, color);
    if (entry == NULL) {
        entry = tile_sprite_add(tile_id, color);
    }
    if (entry == NULL) {
        tile_sprite_clear();
        entry = tile_sprite_add(tile_id, color);
    }

    assert(entry != NULL);

    return entry;
}

/**
 * Returns the next entry in the list, or null if the deleted entry is the
 * last.
 */
static struct tile_sprite_entry_t *
tile_sprite_erase(struct tile_sprite_entry_t *entry)
{
    struct tile_sprite_entry_t *next;

    next = entry->next;
    destroy_bitmap(entry->bmp);
    pslab_erase_elem(&tile_data.sprite_pslab, entry);

    return next;
}

void
tile_sprite_clear(void)
{
    struct tile_sprite_entry_t *entry;
    int i;

    for (i = 0; i < TILE_SPRITE_HASH_SZ; i++) {
        entry = tile_data.sprite_hash[i];
        while (entry != NULL) {
            entry = tile_sprite_erase(entry);
        }
        tile_data.sprite_hash[i] = NULL;
    }
}

struct BITMAP *
tile_to_bitmap(int tile_id, uint32_t color)
{
    struct BITMAP *bmp;

    bmp = create_bitmap(TILE_DIM, TILE_DIM);
    assert(bmp != NULL);
    clear_to_color(bmp, COLOR_TRANS);
    tile_draw_no_alpha_raw(bmp, 0, 0, tile_id, color);

    return bmp;
}

void
tile_draw_no_alpha_raw(struct BITMAP *bmp, int bmpx, int bmpy,
                       tile_id_t tile_id, uint32_t color)
{
    tile_draw_bg_raw(bmp, bmpx, bmpy, tile_id, color, -1);
}

void
tile_draw_bg_raw(struct BITMAP *bmp, int bmpx, int bmpy, tile_id_t tile_id,
                 uint32_t color, uint32_t bg)
{
    /* If the background color isn't transparent, fill the background.  This
     * shouldn't be necessary; allegro appears to be buggy.
     */
    if (bg != (uint32_t)-1) {
        draw_character_ex(bmp, tile_data.tiles[TILE_ID_BLOCK_100], bmpx, bmpy,
                          bg, 0);
    }

    draw_character_ex(bmp, tile_data.tiles[tile_id], bmpx, bmpy, color, bg);
}

void
tile_draw_bg(struct canvas_t *canvas, int canvasx, int canvasy,
             tile_id_t tile_id, uint32_t color, uint32_t bg)
{
    int bmpx;
    int bmpy;

    bmpx = canvasx + canvas->x;
    bmpy = canvasy + canvas->y;
    tile_draw_bg_raw(canvas->bmp, bmpx, bmpy, tile_id, color, bg);
}

void
tile_draw_no_alpha(struct canvas_t *canvas, int canvasx, int canvasy,
                   tile_id_t tile_id, uint32_t color)
{
    tile_draw_bg(canvas, canvasx, canvasy, tile_id, color, -1);
}

void
tile_draw(struct canvas_t *canvas, int canvasx, int canvasy, tile_id_t tile_id,
          uint32_t color, struct color_info_t *color_info)
{
    struct tile_sprite_entry_t *entry;
    struct color_info_t color_info_lcl;
    int bmpx;
    int bmpy;

    if (color_info != NULL && color_info->mode == COLOR_MODE_NONE) {
        color_info = NULL;
    }

    if (color_info == NULL && geta(color) != 255) {
        color_info = &color_info_lcl;
        color_info_lcl.mode = COLOR_MODE_TRANS;
        color_info_lcl.r = 0;
        color_info_lcl.g = 0;
        color_info_lcl.b = 0;
        color_info_lcl.a = geta(color);
    }

    if (color_info != NULL) {
        bmpx = canvasx + canvas->x;
        bmpy = canvasy + canvas->y;
        entry = tile_sprite_ensure(tile_id, color);
        gfx_smart_draw(canvas->bmp, entry->bmp, bmpx, bmpy,
                       color_info, 1.0, 0, 0, 0);
    } else {
        tile_draw_no_alpha(canvas, canvasx, canvasy, tile_id, color);
    }
}

static BITMAP *
tile_from_bitmap(struct BITMAP *bmp, int x, int y)
{
    struct BITMAP *tile;
    int color;
    int tilex;
    int tiley;

    tile = create_bitmap_ex(8, TILE_DIM, TILE_DIM);
    assert(tile != NULL);

    for (tiley = 0; tiley < TILE_DIM; tiley++) {
        for (tilex = 0; tilex < TILE_DIM; tilex++) {
            if (getpixel(bmp, x + tilex, y + tiley) != 0) {
                color = 1;
            } else {
                color = 0;
            }

            putpixel(tile, tilex, tiley, color);
        }
    }

    return tile;
}

void
tile_init(void)
{
    struct BITMAP *bmp;
    char *path;
    int x;
    int y;
    int i;

    assert(TILE_DIM == 8);

    path = res_bmp_path("ascii.pcx");
    bmp = load_bitmap(path, NULL);
    assert(bmp != NULL);

    assert(bmp->w % TILE_DIM == 0);
    assert(bmp->h % TILE_DIM == 0);
    assert(bmp->w / TILE_DIM * bmp->h / TILE_DIM == TILE_MAX);

    x = 0;
    y = 0;
    for (i = 0; i < TILE_MAX; i++) {
        tile_data.tiles[i] = tile_from_bitmap(bmp, x, y);

        x += TILE_DIM;
        if (x >= bmp->w) {
            x = 0;
            y += TILE_DIM;
        }
    }

    pslab_create(&tile_data.sprite_pslab, sizeof (struct tile_sprite_entry_t),
                 TILE_SPRITE_MAX_ENTRIES);
}


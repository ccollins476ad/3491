#include <assert.h>
#include "gen/gen_dbg.h"
#include "gfx/balleg.h"
#include "gfx/resize.h"
#include "gfx/smart_resize.h"

static int
smart_resize_pixel_is_trans(BITMAP *bmp, int x, int y)
{
    uint32_t pixel;

    pixel = getpixel(bmp, x, y);
    return getr(pixel) == 255 && getg(pixel) == 0 && getb(pixel) == 255;
}

/**
 * Replaces transparent pixels in source image with black pixels.
 */
static void
smart_resize_clear_trans(BITMAP *bmp)
{
    int x;
    int y;

    for (y = 0; y < bmp->h; ++y) {
        for (x = 0; x < bmp->w; ++x) {
            if (smart_resize_pixel_is_trans(bmp, x, y)) {
                _putpixel32(bmp, x, y, 0);
            }
        }
    }
}

/**
 * Copies transparent pixels from the source to the destination bitmap.
 * Non-transparent pixels are ignored.
 */
static void
smart_resize_copy_trans(BITMAP *src, BITMAP *dst)
{
    assert(src->w == dst->w && src->h == dst->h);

    int x;
    int y;

    for (y = 0; y < src->h; ++y) {
        for (x = 0; x < src->w; ++x) {
            if (smart_resize_pixel_is_trans(src, x, y)) {
                _putpixel32(dst, x, y, makecol(255, 0, 255));
            }
        }
    }
}

/**
 * Performs an image resize while respecting transparency.
 */
struct BITMAP *
smart_resize(struct BITMAP *src, int neww, int newh)
{
    BITMAP *stretched;
    BITMAP *cleared;
    BITMAP *resized;

    cleared = create_bitmap(src->w, src->h);
    stretched = create_bitmap(neww, newh);
    resized = create_bitmap(neww, newh);

    set_clip_state(cleared, 0);
    set_clip_state(stretched, 0);
    set_clip_state(resized, 0);

    if (cleared == NULL || stretched == NULL || resized == NULL) {
        goto done;
    }

    clear_to_color(resized, makecol(255, 0, 255));

    /* cleared = source bmp with transparent replaced with black. */
    blit(src, cleared, 0, 0, 0, 0, src->w, src->h);
    smart_resize_clear_trans(cleared);

    /* stretched = primitive resize of source bmp. */
    stretch_blit(src, stretched, 0, 0, src->w, src->h, 0, 0, neww, newh);

    /* resized = advanced resize of cleared. */
    resize_bitmap(cleared, resized);

    /* Copy transparent pixels from stretched to resized. */
    smart_resize_copy_trans(stretched, resized);

done:
    destroy_bitmap(cleared);
    destroy_bitmap(stretched);

    return resized;
}


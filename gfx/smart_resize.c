#include <assert.h>
#include "balleg.h"
#include "resize.h"
#include "smart_resize.h"

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
                putpixel(bmp, x, y, 0);
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
                putpixel(dst, x, y, makecol(255, 0, 255));
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

    if (cleared == NULL || stretched == NULL || resized == NULL) {
        goto done;
    }

    blit(src, cleared, 0, 0, 0, 0, src->w, src->h);
    smart_resize_clear_trans(cleared);

    stretch_blit(src, stretched, 0, 0, src->w, src->h, 0, 0, neww, newh);

    resize_bitmap(cleared, resized);
    smart_resize_copy_trans(stretched, resized);

done:
    destroy_bitmap(cleared);
    destroy_bitmap(stretched);

    return resized;
}


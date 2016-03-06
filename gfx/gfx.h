#ifndef H_GFX_
#define H_GFX_

#include <inttypes.h>
struct BITMAP;

#define COLORA(r, g, b, a) \
    ((a) << 24 | (r) << 16 | (g) << 8 | (b))

#define COLOR(r, g, b) COLORA((r), (g), (b), 255)

#define RED(r)      COLOR((r), 0, 0)
#define GREEN(g)    COLOR(0, (g), 0)
#define BLUE(b)     COLOR(0, 0, (b))
#define WHITE(c)    COLOR((c), (c), (c))
#define YELLOW(y)   COLOR((y), (y), 0)

#define REDA(r, a) COLORA((r), 0, 0, (a))
#define WHITEA(w, a) COLORA((w), (w), (w), (a))

#define COLOR_NONE  (-2)
#define COLOR_TRANS COLORA(0xff, 0, 0xff, 0)

#define COLOR_MODE_NONE     0
#define COLOR_MODE_LIT      1
#define COLOR_MODE_TRANS    2

struct color_info_t {
    int mode;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

typedef int gfx_draw_cb_t(int x, int y, void *arg);

void gfx_set_default_blender(void);
int circlefill_foreach(int x, int y, int radius, gfx_draw_cb_t *cb, void *arg);
int line_foreach(int x1, int y1, int x2, int y2, gfx_draw_cb_t *cb, void *arg);

void gfx_smart_draw(struct BITMAP *dst, struct BITMAP *src, int x, int y,
                    struct color_info_t *color_info,
                    double scale, double angle, int hflip, int vflip);

void gfx_init(void);

#endif


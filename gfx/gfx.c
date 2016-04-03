#include <assert.h>
#include <math.h>
#include "gfx/balleg.h"
#include "gfx/gfx.h"

static const double ALLEGRO_ANGLE_CONVERSION_FACTOR = -256.0 / (2 * M_PI);

static int
hline_foreach(int x1, int y, int x2, gfx_draw_cb_t *cb, void *arg)
{
    int rc;
    int x;

    if (x1 > x2) {
        x = x1;
        x1 = x2;
        x2 = x;
    }

    for (x = x1; x <= x2; x++) {
        rc = cb(x, y, arg);
        if (rc != 0) {
            return rc;
        }
    }

    return 0;
}

static int
vline_foreach(int x, int y1, int y2, gfx_draw_cb_t *cb, void *arg)
{
    int rc;
    int y;

    if (y1 > y2) {
        y = y1;
        y1 = y2;
        y2 = y;
    }

    for (y = y1; y <= y2; y++) {
        rc = cb(x, y, arg);
        if (rc != 0) {
            return rc;
        }
    }

    return 0;
}

void
gfx_set_default_blender(void)
{
    set_alpha_blender();
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
}

/**
 * Adapted from allegro 4 code.
 */
int
line_foreach(int x1, int y1, int x2, int y2, gfx_draw_cb_t *cb, void *arg)
{
    int dx = x2-x1;
    int dy = y2-y1;
    int i1, i2;
    int x, y;
    int dd;
    int rc;

    if (x1 == x2) {
        return vline_foreach(x1, y1, y2, cb, arg);
    }
    if (y1 == y2) {
        return hline_foreach(x1, y1, x2, cb, arg);
    }

    /* worker macro */
    #define DO_LINE(pri_sign, pri_c, pri_cond, sec_sign, sec_c, sec_cond) do \
    {                                                                        \
        if (d##pri_c == 0) {                                                 \
            return cb(x1, y1, arg);                                          \
        }                                                                    \
                                                                             \
        i1 = 2 * d##sec_c;                                                   \
        dd = i1 - (sec_sign (pri_sign d##pri_c));                            \
        i2 = dd - (sec_sign (pri_sign d##pri_c));                            \
                                                                             \
        x = x1;                                                              \
        y = y1;                                                              \
                                                                             \
        while (pri_c pri_cond pri_c##2) {                                    \
            rc = cb(x, y, arg);                                              \
            if (rc != 0) {                                                   \
                return rc;                                                   \
            }                                                                \
                                                                             \
            if (dd sec_cond 0) {                                             \
                sec_c = sec_c sec_sign 1;                                    \
                dd += i2;                                                    \
            } else {                                                         \
                dd += i1;                                                    \
            }                                                                \
                                                                             \
            pri_c = pri_c pri_sign 1;                                        \
        }                                                                    \
    } while (0)

    if (dx >= 0) {
        if (dy >= 0) {
            if (dx >= dy) {
                /* (x1 <= x2) && (y1 <= y2) && (dx >= dy) */
                DO_LINE(+, x, <=, +, y, >=);
            } else {
                /* (x1 <= x2) && (y1 <= y2) && (dx < dy) */
                DO_LINE(+, y, <=, +, x, >=);
            }
        } else {
            if (dx >= -dy) {
                /* (x1 <= x2) && (y1 > y2) && (dx >= dy) */
                DO_LINE(+, x, <=, -, y, <=);
            } else {
                /* (x1 <= x2) && (y1 > y2) && (dx < dy) */
                DO_LINE(-, y, >=, +, x, >=);
            }
        }
    } else {
        if (dy >= 0) {
            if (-dx >= dy) {
                /* (x1 > x2) && (y1 <= y2) && (dx >= dy) */
                DO_LINE(-, x, >=, +, y, >=);
            } else {
                /* (x1 > x2) && (y1 <= y2) && (dx < dy) */
                DO_LINE(+, y, <=, -, x, <=);
            }
        } else {
            if (-dx >= -dy) {
                /* (x1 > x2) && (y1 > y2) && (dx >= dy) */
                DO_LINE(-, x, >=, -, y, <=);
            } else {
                /* (x1 > x2) && (y1 > y2) && (dx < dy) */
                DO_LINE(-, y, >=, -, x, <=);
            }
        }
    }
#undef DO_LINE

    return 0;
}

/**
 * Adapted from allegro 4 code.
 */
int
circlefill_foreach(int x, int y, int radius, gfx_draw_cb_t *cb, void *arg)
{
    int d_se;
    int d_e;
    int cx;
    int cy;
    int df;
    int rc;

    cx = 0;
    cy = radius;
    df = 1 - radius; 
    d_e = 3;
    d_se = -2 * radius + 5;

    do {
        rc = hline_foreach(x - cy, y - cx, x + cy, cb, arg);
        if (rc != 0) {
            return rc;
        }

        if (cx) {
            rc = hline_foreach(x - cy, y + cx, x + cy, cb, arg);
            if (rc != 0) {
                return rc;
            }
        }

        if (df < 0) {
            df += d_e;
            d_e += 2;
            d_se += 2;
        } else { 
            if (cx != cy) {
                rc = hline_foreach(x - cx, y - cy, x + cx, cb, arg);
                if (rc != 0) {
                    return rc;
                }

                if (cy) {
                    rc = hline_foreach(x - cx, y + cy, x + cx, cb, arg);
                    if (rc != 0) {
                        return rc;
                    }
                }
            }

            df += d_se;
            d_e += 2;
            d_se += 4;
            cy--;
        }

        cx++; 
    } while (cx <= cy);

    return 0;
}

static fixed
to_allegro_angle(double angle)
{
    return itofix(angle * ALLEGRO_ANGLE_CONVERSION_FACTOR);
}

static void
process_flip_angle(int hflip, int *vflip, double *angle)
{
    if (hflip) {
        *angle += M_PI;
        *vflip = !vflip;
    }
}

void
gfx_smart_draw(BITMAP *dst, BITMAP *src, int x, int y,
               struct color_info_t *color_info,
               double scale, double angle, int hflip, int vflip)
{
    enum smart_code_t {
        HFLIP = 0x01,
        VFLIP = 0x02,
        SCALE = 0x04,
        ANGLE = 0x08,
    };

    fixed fixed_scale;
    fixed fixed_angle;
    uint32_t color;
    int mode;
    int code;
    int al_flip;

    code = 0;
    if (scale != 1.0)   code |= SCALE;
    if (angle != 0.0)   code |= ANGLE;

    if (color_info != NULL) {
        switch (color_info->mode) {
        case COLOR_MODE_NONE:
            mode = DRAW_SPRITE_NORMAL;
            break;

        case COLOR_MODE_LIT:
            mode = DRAW_SPRITE_LIT;
            set_add_blender(color_info->r, color_info->g,
                            color_info->b, color_info->a);
            break;

        case COLOR_MODE_TRANS:
            mode = DRAW_SPRITE_TRANS;
            set_trans_blender(color_info->r, color_info->g,
                              color_info->b, color_info->a);
            break;

        default:
            assert(0);
            mode = DRAW_SPRITE_NORMAL;
            break;
        }
    } else {
        mode = DRAW_SPRITE_NORMAL;
    }

    if (hflip) {
        if (vflip) {
            al_flip = DRAW_SPRITE_VH_FLIP;
        } else {
            al_flip = DRAW_SPRITE_H_FLIP;
        }
    } else if (vflip) {
        al_flip = DRAW_SPRITE_V_FLIP;
    } else {
        al_flip = DRAW_SPRITE_NO_FLIP;
    }

    switch (code) {
    case 0:
        draw_sprite_ex(dst, src, x, y, mode, al_flip);
        break;

    case SCALE:
        if (mode == DRAW_SPRITE_NORMAL) {
            stretch_sprite(dst, src, x, y, src->w * scale, src->h * scale);
            break;
        }
        /* Else, fall through. */

    default:
        process_flip_angle(hflip, &vflip, &angle);
        fixed_angle = to_allegro_angle(angle);
        fixed_scale = ftofix(scale);

        switch (mode) {
        case DRAW_SPRITE_NORMAL:
            if (vflip) {
                rotate_scaled_sprite_v_flip(dst, src, x, y, fixed_angle,
                                            fixed_scale);
            } else {
                rotate_scaled_sprite(dst, src, x, y, fixed_angle, fixed_scale);
            }
            break;

        case DRAW_SPRITE_LIT:
            color = COLORA(color_info->r, color_info->g, color_info->b,
                           color_info->a);
            if (vflip) {
                rotate_scaled_sprite_v_flip_lit(dst, src, x, y, fixed_angle,
                                                fixed_scale, color);
            } else {
                rotate_scaled_sprite_lit(dst, src, x, y, fixed_angle,
                                         fixed_scale, color);
            }
            break;

        case DRAW_SPRITE_TRANS:
            if (vflip) {
                rotate_scaled_sprite_v_flip_trans(dst, src, x, y, fixed_angle,
                                                  fixed_scale);
            } else {
                rotate_scaled_sprite_trans(dst, src, x, y, fixed_angle,
                                           fixed_scale);
            }
            break;

        default:
            assert(0);
            break;
        }
    }

    /* Undo any global blender changes. */
    if (mode != DRAW_SPRITE_NORMAL) {
        gfx_set_default_blender();
    }
}

void
gfx_init(void)
{
    gfx_set_default_blender();
}


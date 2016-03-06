#include "gfx/gfx.h"
#include "gfx/fader.h"

void
fader_update(struct fader_t *fader)
{
    int new_a;

    if (fader->color_info.mode == COLOR_MODE_NONE) {
        return;
    }

    new_a = fader->color_info.a + fader->delta;
    if (new_a <= 0) {
        if (fader->color_info.mode == COLOR_MODE_LIT) {
            fader->color_info.mode = COLOR_MODE_NONE;
        }
        new_a = 0;
    } else if (new_a > 255) {
        new_a = 255;
    }

    fader->color_info.a = new_a;
}


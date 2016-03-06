#include <assert.h>
#include "balleg.h"
#include "screen.h"

struct BITMAP *screen_buf;

int
screen_init(int width, int height, int depth_bits, int windowed)
{
    int mode;
    int rc;

    /* Don't allow this to be called more than once. */
    assert(screen_buf == NULL);

    set_color_depth(depth_bits);

    if (windowed) {
        mode = GFX_AUTODETECT_WINDOWED;
    } else {
        mode = GFX_AUTODETECT_FULLSCREEN;
    }

    rc = set_gfx_mode(mode, width, height, 0, 0);
    if (rc != 0) {
        return -1;
    }

    screen_buf = create_bitmap(width, height);
    if (screen_buf == NULL) {
        return -1;
    }

    return 0;
}

void    
screen_draw(void)
{
    blit(screen_buf, screen, 0, 0, 0, 0, screen->w, screen->h);
}


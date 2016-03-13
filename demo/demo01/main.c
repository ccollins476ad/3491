#include <stdio.h>
#include <assert.h>
#include "gfx/balleg.h"
#include "gfx/canvas.h"
#include "gfx/gfx.h"
#include "gfx/screen.h"
#include "life/data.h"
#include "life/image.h"
#include "life/life_defs.h"
#include "life/res.h"
#include "life/timing.h"

static void
init(void)
{
    int rc;

    rc = allegro_init();
    if (rc != 0) {
        fprintf(stderr, "allegro init failure; rc=%d\n", rc);
        exit(EXIT_FAILURE);
    }

    rc = screen_init(640, 480, 32, 1);
    if (rc != 0) {
        fprintf(stderr, "screen init failure; rc=%d\n", rc);
        exit(EXIT_FAILURE);
    }

    rc = install_keyboard();
    if (rc != 0) {
        fprintf(stderr, "install keyboard failure; rc=%d\n", rc);
        exit(EXIT_FAILURE);
    }

    timing_init(LIFE_FPS);
}

#ifdef _WIN32
static
#endif
int
main(void)
{
    struct canvas_t canvas;
    struct image_t *image;
    struct BITMAP *bmp;
    int draw_frame;

    init();

    memset(&canvas, 0, sizeof canvas);
    canvas.dimx = 640;
    canvas.dimy = 480;
    canvas.bmp = screen_buf;

    image = data_images + IMAGE_ID_PP_STRAIGHT;
    bmp = image_bmp(image);

    draw_frame = 0;
    while (1) {
        while (timing_tick > 0) {
            //input_update();
            //demo01_process_input();
            canvas_set_clip(&canvas);
            //world_draw_info(&wdi, &view, &canvas);

            draw_frame = 1;
            --timing_tick;
        }

        if (draw_frame) {
            gfx_smart_draw(canvas.bmp, bmp, 100, 100, NULL, 1.0, 0.0, 0, 0);

            screen_draw();
            draw_frame = 0;
            rest(0);
        } else {
            rest(1);
        }
    }

    return 0;
}
END_OF_MAIN()


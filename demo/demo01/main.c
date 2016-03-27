#include <stdio.h>
#include <assert.h>
#include "gen/res.h"
#include "gfx/balleg.h"
#include "gfx/canvas.h"
#include "gfx/gfx.h"
#include "gfx/screen.h"
#include "life/data.h"
#include "life/image.h"
#include "life/life_defs.h"
#include "life/being.h"
#include "life/scroll.h"
#include "life/world.h"
#include "life/level.h"
#include "life/timing.h"

struct level_t demo01_level_1 = {
    .tiles = (struct level_tile_t[]) {
        { 'X', TERR_ID_STEEL_WALL },
        { '~', TERR_ID_DIRT },
        { '.', TERR_ID_AIR_BLACK },
    },

    .rows = (char *[]) {
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
        "X..............................X",
        "X..............................X",
        "X..............................X",
        "X..............................X",
        "X..............................X",
        "X..............................X",
        "X..............................X",
        "X..............................X",
        "X..............................X",
        "X..............................X",
        "XXXXXXXXXXXXXXX.XXXXXXXXXXXXXXXX",
        "X.........~........X........XXXX",
        "X.........~........X........XXXX",
        "X.........~........X...X....XXXX",
        "X.........~........X...XX...XXXX",
        "X........~.........X...XX...XXXX",
        "X........~.........X........XXXX",
        "X..~~..~~..........X........XXXX",
        "X~~..~~.....................XXXX",
        "X~.................X........XXXX",
        "X~~.............X.XXXXXXXXXXXXXX",
        "X~..............X..XXXXXXXXXXXXX",
        "X...............X..XXXXXXXXXXXXX",
        "XXXXXXXXXXXX..XXXXXXXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "XXXXXXXXX..........XXXXXXXXXXXXX",
        "X...X...X..........XXXXXXXXXXXXX",
        "X...X...X..........XXXXXXXXXXXXX",
        "X...X...X..........XXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "X...X...X..........XXXXXXXXXXXXX",
        "X...X...X..........XXXXXXXXXXXXX",
        "X...X...X..........XXXXXXXXXXXXX",
        "XXXXXXXXX.......XX.XXXXXXXXXXXXX",
        "X...............X..XXXXXXXXXXXXX",
        "X...............X..XXXXXXXXXXXXX",
        "X...............X..XXXXXXXXXXXXX",
        "X...............X..XXXXXXXXXXXXX",
        "X...............X..XXXXXXXXXXXXX",
        "X..............XXXXXXXXXXXXXXXXX",
        "X..............XXX.XXXXXXXXXXXXX",
        "X..............XXX.XXXXXXXXXXXXX",
        "X...............XX.XXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "X..................XXXXXXXXXXXXX",
        "XXXXXXXXXXXXXX...XXXXXXXXXXXXXXX",
        "...................XXXXXXXXXXXXX",
        ".............XXXXXXXXXXXXXXXXXXX",
        "................................",
        "................................",
        "................................",
        "................................",
        "................................",
        "................................",
        "................................",
        NULL,
    },
};

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

    scroll_data.mode = SCROLL_MODE_FREE;

    tile_init();
    gfx_init();


    timing_init(LIFE_FPS);
    level_load(&demo01_level_1);
}

#ifdef _WIN32
static
#endif
int
main(void)
{
    struct canvas_t canvas;
    struct being_t being;
    struct BITMAP *bmp;
    int draw_frame;

    init();

    memset(&canvas, 0, sizeof canvas);
    canvas.dimx = 640;
    canvas.dimy = 480;
    canvas.bmp = screen_buf;

    world_view_set_canvas(&canvas);

    memset(&being, 0, sizeof being);
    being.image = data_images + IMAGE_ID_PP_STRAIGHT;
    being.phys.x = 100;
    being.phys.y = 100;

    draw_frame = 0;
    while (1) {
        while (timing_tick > 0) {
            //input_update();
            //demo01_process_input();
            canvas_set_clip(&canvas);
            scroll_update();

            //world_draw_info(&wdi, &view, &canvas);

            draw_frame = 1;
            --timing_tick;

            //playerx++;
            //playery++;
            being.angle++;
        }

        if (draw_frame) {
            terr_draw_scape();
            bmp = image_bmp(being.image);
            gfx_smart_draw(canvas.bmp, bmp, being.phys.x, being.phys.y,
                           NULL, 1.0, being.angle, 0, 0);
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

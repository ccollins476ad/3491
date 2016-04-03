#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "gen/gen_dbg.h"
#include "gen/gen_num.h"
#include "gen/res.h"
#include "gfx/balleg.h"
#include "gfx/canvas.h"
#include "gfx/gfx.h"
#include "gfx/screen.h"
#include "life/being.h"
#include "life/bullet.h"
#include "life/carnage.h"
#include "life/data.h"
#include "life/event.h"
#include "life/gesture.h"
#include "life/image.h"
#include "life/input.h"
#include "life/level.h"
#include "life/life_defs.h"
#include "life/object.h"
#include "life/scroll.h"
#include "life/timing.h"
#include "life/world.h"

struct level_t demo01_level_1 = {
    .tiles = (struct level_tile_t[]) {
        { 'X', TERR_ID_STEEL_WALL },
        { 'o', TERR_ID_LIT_WINDOW },
        { '-', TERR_ID_OFF_WINDOW },
        { '~', TERR_ID_DIRT },
        { '.', TERR_ID_AIR_BLACK },
        { '@', TERR_ID_GRAY_RUBBLE},
    },

    .rows = (char *[]) {
        "................................",
        "................................",
        "................................",
        "................................",
        "................................",
        "................................",
        "................................",
        "................................",
        "......@.........................",
        "XXXXXXXXXXX.....................",
        "XoXoX-XoXoX.....................",
        "XXXXXXXXXXX.....................",
        "XoXoXoX-X-X.....................",
        "XXXXXXXXXXX.....................",
        "X-XoXoXoXoX.....................",
        "XXXXXXXXXXX.....................",
        "X-X-X-XoXoX.....................",
        "XXXXXXXXXXX...XXXXXXXXXXXXXXX...",
        "X-XoXoXoXoX...X-XoXoX-XoXoX-X...",
        "XXXXXXXXXXX...XXXXXXXXXXXXXXX...",
        "XoXoXoX-XoX...XoXoXoXoXoX-XoX...",
        "XXXXXXXXXXX...XXXXXXXXXXXXXXX...",
        "XoXoX-XoXoX...XoXoXoXoXoXoX-X...",
        "XXXXXXXXXXX...XXXXXXXXXXXXXXX...",
        "XoXoXoXoX-X...XoX-X-XoXoXoX-X...",
        "XXXXXXXXXXX...XXXXXXXXXXXXXXX...",
        "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
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

    rc = input_init();
    assert(rc == 0);
    input_start();

    data_gesture_init();
    tile_init();
    gfx_init();
    bullet_init();
    hit_box_init();
    entity_init();
    event_init();
    being_init();
    blood_init();
    spark_init();
    combat_init();
    object_init();
    terr_init();
    carnage_init();
    level_load(&demo01_level_1);

    timing_init(LIFE_FPS);

    scroll_data.mode = SCROLL_MODE_FREE;
}

#ifdef _WIN32
static
#endif
int
main(void)
{
    struct gesture_set_t gesture_set;
    struct canvas_t canvas;
    struct being_t *being;
    int draw_frame;

    init();

    gesture_set_create(&gesture_set);

    memset(&canvas, 0, sizeof canvas);
    canvas.dimx = 640;
    canvas.dimy = 480;
    canvas.bmp = screen_buf;

    being = being_add(BEING_TYPE_RALF);
    assert(being != NULL);

    being->image = data_images + IMAGE_ID_PP_STRAIGHT;
    being->phys.x = 100;
    being->phys.y = 100;
    being->log_ctxt.fsm = 1;
    being->flags |= BEING_F_PLAYER;

    world_view_set_canvas(&canvas);

    draw_frame = 0;
    while (1) {
        while (timing_tick > 0) {
            if (key[KEY_ESC]) {
                return 0;
            }

            world_fill_zone();

            input_update();
            //demo01_process_input();
            input_state_update_action();

            gesture_detect(data_gestures, &gesture_set);
            gesture_set_apply(&gesture_set, &being->fsm_ctxt,
                              data_gesture_fsm_map_basic);

            being_batch_update();

            canvas_set_clip(&canvas);
            scroll_update();

            draw_frame = 1;
            --timing_tick;
        }

        if (draw_frame) {
            terr_draw_scape();
            being_batch_set_visible(being->phys.x, being->phys.y);
            being_batch_draw();

            //if (game_dbg_state.draw_hit_boxes) {
                //hit_box_draw_all();
            //}

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

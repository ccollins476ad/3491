#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include "gfx/balleg.h"
#include "gfx/canvas.h"
#include "gfx/screen.h"
#include "gfx/tile.h"
#include "gfx/icon.h"
#include "gfx/gfx.h"

#define IMGED_INPUT_NONE            -1
#define IMGED_INPUT_ESC             0
#define IMGED_INPUT_UP              1
#define IMGED_INPUT_RIGHT           2
#define IMGED_INPUT_DOWN            3
#define IMGED_INPUT_LEFT            4
#define IMGED_INPUT_SPACE           5
#define IMGED_INPUT_COUNT           6

#define IMGED_WIDTH                 80
#define IMGED_HEIGHT                60

struct imged_input_entry_t {
    unsigned int scancode;
    int input_id;
};

static struct imged_input_entry_t imged_input_map[IMGED_INPUT_COUNT] = {
    { KEY_ESC  << 8,     IMGED_INPUT_ESC },
    { KEY_UP << 8,       IMGED_INPUT_UP },
    { KEY_RIGHT << 8,    IMGED_INPUT_RIGHT },
    { KEY_DOWN << 8,     IMGED_INPUT_DOWN },
    { KEY_LEFT << 8,     IMGED_INPUT_LEFT },
    { KEY_SPACE << 8,    IMGED_INPUT_SPACE },
};

static struct canvas_t imged_canvas;
static int imged_quit;
static int imged_x;
static int imged_y;

static struct icon_t imged_img[IMGED_HEIGHT][IMGED_WIDTH];

typedef void imged_input_fn_t(int input_id);

static imged_input_fn_t imged_input_esc;
static imged_input_fn_t imged_input_up;
static imged_input_fn_t imged_input_right;
static imged_input_fn_t imged_input_down;
static imged_input_fn_t imged_input_left;
static imged_input_fn_t imged_input_space;

static imged_input_fn_t *imged_input_dispatch[IMGED_INPUT_COUNT] = {
    [IMGED_INPUT_ESC] =     imged_input_esc,
    [IMGED_INPUT_UP] =      imged_input_up,
    [IMGED_INPUT_RIGHT] =   imged_input_right,
    [IMGED_INPUT_DOWN] =    imged_input_down,
    [IMGED_INPUT_LEFT] =    imged_input_left,
    [IMGED_INPUT_SPACE] =   imged_input_space,
};

static void
imged_draw_tile(int tilex, int tiley, tile_id_t tile_id, uint32_t color)
{
    int canvasx;
    int canvasy;

    canvasx = tilex * TILE_DIM;
    canvasy = tiley * TILE_DIM;

    tile_draw(&imged_canvas, canvasx, canvasy, tile_id, color, NULL);
}

static void
erase_cursor(void)
{
    struct icon_t *icon;

    assert(imged_x >= 0 && imged_x < IMGED_WIDTH);
    assert(imged_y >= 0 && imged_y < IMGED_HEIGHT);

    icon = &imged_img[imged_y][imged_x];
    imged_draw_tile(imged_x, imged_y, icon->tile_id, icon->color);
}

static void
draw_cursor(int x, int y)
{
    imged_draw_tile(imged_x, imged_y, TILE_ID_BLOCK_100, WHITE(255));
}

static void
imged_set_cursor_loc(int x, int y)
{
    erase_cursor();
    imged_x = x;
    imged_y = y;
}

static void
imged_clear(void)
{
    int x;
    int y;

    for (y = 0; y < IMGED_HEIGHT; y++) {
        for (x = 0; x < IMGED_WIDTH; x++) {
            imged_img[y][x].tile_id = TILE_ID_BLOCK_100;
            imged_img[y][x].color = WHITE(0);
        }
    }
}

static void
imged_input_esc(int input_id)
{
    imged_quit = 1;
}

static void
imged_input_up(int input_id)
{
    imged_set_cursor_loc(imged_x, imged_y - 1);
}

static void
imged_input_right(int input_id)
{
    imged_set_cursor_loc(imged_x + 1, imged_y);
}

static void
imged_input_down(int input_id)
{
    imged_set_cursor_loc(imged_x, imged_y + 1);
}

static void
imged_input_left(int input_id)
{
    imged_set_cursor_loc(imged_x - 1, imged_y);
}

static void
imged_input_space(int input_id)
{
    imged_img[imged_y][imged_x].tile_id = TILE_ID_BLOCK_100;
    imged_img[imged_y][imged_x].color = RED(255);
}

static int
imged_input_lookup(unsigned int scancode)
{
    struct imged_input_entry_t *entry;
    int i;

    for (i = 0; i < sizeof imged_input_map / sizeof imged_input_map[0]; i++) {
        entry = imged_input_map + i;
        if ((scancode & 0xff00) == (entry->scancode & 0xff00)) {
            return entry->input_id;
        }
    }

    return IMGED_INPUT_NONE;
}

static int
imged_input_next(void)
{
    unsigned int scancode;
    int input_id;

    scancode = readkey();
    input_id = imged_input_lookup(scancode);
    return input_id;
}

static imged_input_fn_t *
imged_input_dispatch_lookup(int input_id)
{
    imged_input_fn_t *cb;

    if (input_id == IMGED_INPUT_NONE) {
        cb = NULL;
    } else {
        cb = imged_input_dispatch[input_id];
    }

    return cb;
}

/* Returns 0 if input was successfully handled. */
static int
imged_input_process(void)
{
    imged_input_fn_t *cb;
    int input_id;

    input_id = imged_input_next();
    cb = imged_input_dispatch_lookup(input_id);
    if (cb == NULL) {
        return ENOENT;
    }

    cb(input_id);
    return 0;
}

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

    tile_init();
    gfx_init();

    imged_canvas.bmp = screen_buf;

    imged_clear();
}

#ifdef _WIN32
static
#endif
int
main(void)
{
    init();

    while (!imged_quit) {
        draw_cursor(imged_x, imged_y);
        screen_draw();

        while (imged_input_process() != 0)
            ;
    }

    return 0;
}
END_OF_MAIN()

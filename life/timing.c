#include <assert.h>
#include "gfx/balleg.h"
#include "life/timing.h"

#define TIMING_TICK_MAX 1

volatile unsigned int timing_tick;
volatile unsigned int timing_action_tick;

int timing_action_fps;

static void
timing_int(void)
{
    if (timing_tick < TIMING_TICK_MAX) {
        timing_tick++;
    }
}
END_OF_FUNCTION(timing_int);

static void
timing_action_int(void)
{
    if (timing_action_tick < TIMING_TICK_MAX) {
        timing_action_tick++;
    }
}
END_OF_FUNCTION(timing_action_int);

void
timing_set_action_fps(int fps)
{
    int rc;

    rc = install_int_ex(timing_action_int, BPS_TO_TIMER(fps));
    assert(rc == 0);

    timing_action_fps = fps;
}

void
timing_init(int fps)
{
    int rc;

    rc = install_int_ex(timing_int, BPS_TO_TIMER(fps));
    assert(rc == 0);

    timing_set_action_fps(fps);
}


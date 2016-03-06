#include <assert.h>
#include "gen/direction.h"

static int direction_vector_none[2] = { 0, 0 };

static int direction_vectors[DIRECTION_MAX][2] = {
    [DIRECTION_UP]      = { 0, -1 },
    [DIRECTION_RIGHT]   = { 1, 0 },
    [DIRECTION_DOWN]    = { 0, 1 },
    [DIRECTION_LEFT]    = { -1, 0 },
    [DIRECTION_UR]      = { 1, -1 },
    [DIRECTION_DR]      = { 1, 1 },
    [DIRECTION_DL]      = { -1, 1 },
    [DIRECTION_UL]      = { -1, -1 },
};

static int direction_angles[DIRECTION_MAX] = {
    [DIRECTION_UP]      = 90,
    [DIRECTION_RIGHT]   = 0,
    [DIRECTION_DOWN]    = 270,
    [DIRECTION_LEFT]    = 180,
    [DIRECTION_UR]      = 45,
    [DIRECTION_DR]      = 315,
    [DIRECTION_DL]      = 225,
    [DIRECTION_UL]      = 135,
};

int *
direction_to_vector(int direction)
{
    if (direction == DIRECTION_NONE) {
        return direction_vector_none;
    }

    assert(direction >= 0 && direction < DIRECTION_MAX);
    return direction_vectors[direction];
}

int
direction_from_vector(int x, int y)
{
    int i;

    for (i = 0; i < DIRECTION_MAX; i++) {
        if (direction_vectors[i][0] == x && direction_vectors[i][1] == y) {
            return i;
        }
    }

    return DIRECTION_NONE;
}

int
direction_from_speed(int xspeed, int yspeed)
{
    if (xspeed < 0) {
        xspeed = -1;
    } else if (xspeed > 0) {
        xspeed = 1;
    }
    if (yspeed < 0) {
        yspeed = -1;
    } else if (yspeed > 0) {
        yspeed = 1;
    }

    return direction_from_vector(xspeed, yspeed);
}

int
direction_to_angle(int direction)
{
    assert(direction >= 0 && direction < DIRECTION_MAX);
    return direction_angles[direction];
}


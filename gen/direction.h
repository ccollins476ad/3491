#ifndef DIRECTION_H_
#define DIRECTION_H_

#define DIRECTION_NONE  (-1)
#define DIRECTION_UP    0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN  2
#define DIRECTION_LEFT  3
#define DIRECTION_CARDINAL_MAX  4

#define DIRECTION_UR    4
#define DIRECTION_DR    5
#define DIRECTION_DL    6
#define DIRECTION_UL    7
#define DIRECTION_MAX   8

#define CIRCULAR_NONE   (-1)
#define CIRCULAR_CW     0
#define CIRCULAR_CCW    1
#define CIRCULAR_MAX    2

int *direction_to_vector(int direction);
int direction_from_vector(int x, int y);
int direction_from_speed(int xspeed, int yspeed);
int direction_to_angle(int direction);

#endif


#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include "gen/gen_shape.h"
#include "gen/gen_num.h"

int
min(int a, int b)
{
    return a < b ? a : b;
}

int
min3(int a, int b, int c)
{
    return min(min(a, b), c);
}

int
min4(int a, int b, int c, int d)
{
    return min(min3(a, b, c), d);
}

int
min5(int a, int b, int c, int d, int e)
{
    return min(min4(a, b, c, d), e);
}

int
min6(int a, int b, int c, int d, int e, int f)
{
    return min(min5(a, b, c, d, e), f);
}

int
max(int a, int b)
{
    return a > b ? a : b;
}

int
max3(int a, int b, int c)
{
    return max(max(a, b), c);
}

int
max4(int a, int b, int c, int d)
{
    return max(max3(a, b, c), d);
}

int
max5(int a, int b, int c, int d, int e)
{
    return max(max4(a, b, c, d), e);
}

int
max6(int a, int b, int c, int d, int e, int f)
{
    return max(max5(a, b, c, d, e), f);
}

int
rand_up_to(int max)
{
    if (max == 0) {
        return 0;
    }

    return rand() % max;
}

int
rand_two_ways(int max)
{
    return rand_up_to(max * 2) - max;
}

int
rand_avg(int max)
{
    return (rand_up_to(max) + rand_up_to(max)) / 2;
}

int
distance(int x1, int y1, int x2, int y2)
{
    long long diffxsq;
    long long diffysq;
    long long diffx;
    long long diffy;
    int dist;

    diffx = x1 - x2;
    diffy = y1 - y2;
    diffxsq = diffx * diffx;
    diffysq = diffy * diffy;
    dist = sqrt(diffxsq + diffysq);

    return dist;
}

void
angle_to_normal_vect(double rads, double *x, double *y)
{
    *x = cos(rads);
    *y = -sin(rads);
}

void
angle_to_vect(double rads, int mag, int *x, int *y)
{
    double compx;
    double compy;

    angle_to_normal_vect(rads, &compx, &compy);
    *x = mag * compx;
    *y = mag * compy;
}

double
angle_from_vect(int x, int y)
{
    double rads;
    int opp;
    int adj;

    opp = abs(y);
    adj = abs(x);
    rads = atan((double)opp / adj);

    if (y <= 0) {
        if (x >= 0) {
            /* Upper-right quadrant. */
            /* No change. */
        } else {
            /* Upper-left quadrant. */
            rads = M_PI - rads;
        }
    } else {
        if (x < 0) {
            /* Lower-left quadrant. */
            rads += M_PI;
        } else {
            /* Lower-right quadrant. */
            rads = 2.0 * M_PI - rads;
        }
    }

    return rads;
}

double
rads_normalize(double rads)
{
    rads = fmod(rads, 2 * M_PI);
    if (rads < 0) {
        rads += 2 * M_PI;
    }
    return rads;
}

int
angle_distance_clockwise(int from, int to)
{
    int diff;

    diff = to - from;
    if (diff < 0) {
        diff += 360;
    }

    return diff;
}

int
min_ints(int num_ints, ...)
{
    va_list ap;
    int best;
    int cur;
    int i;

    va_start(ap, num_ints); 

    best = INT_MAX;
    for (i = 0; i < num_ints; i++) {
        cur = va_arg(ap, int);
        if (cur < best) {
            best = cur;
        }
    }

    va_end(ap);

    return best;
}

int
max_ints(int num_ints, ...)
{
    va_list ap;
    int best;
    int cur;
    int i;

    va_start(ap, num_ints); 

    best = INT_MIN;
    for (i = 0; i < num_ints; i++) {
        cur = va_arg(ap, int);
        if (cur > best) {
            best = cur;
        }
    }

    va_end(ap);

    return best;
}

double
circle_distance_ratio(int radius, int dist, double distance_multiplier)
{
    double scaled_ratio;
    double dist_ratio;

    dist_ratio = radius - dist * distance_multiplier;
    if (dist_ratio <= 0.0) {
        dist_ratio = 0.0;
    }
    scaled_ratio = dist_ratio / radius;

    return scaled_ratio;
}

int
div_floor(int n, int d)
{
    return n / d - (n < 0 && n % d != 0 ? 1 : 0);
}

/**
 * A simplified version of strtoul.  Differences from strtoul:
 *   o error code is returned instead of being written to errno.
 *   o parsing fails if the entire source string is not a valid number.
 */
int
simple_strtoul(unsigned long *val, char *src, int base)
{
    char *endptr;
    int rc;

    errno = 0;
    *val = strtoul(src, &endptr, base);
    if (*src == '\0' || *endptr != '\0') {
        rc = EINVAL;
    } else if (*val == ULONG_MAX && errno == ERANGE) {
        rc = ERANGE;
    } else {
        rc = 0;
    }

    return rc;
}


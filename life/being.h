#ifndef H_BEING_
#define H_BEING_

#include "life/phys.h"
struct image_t;

struct being_t {
    struct phys_t phys;
    int angle; /* Degrees. */
    struct image_t *image;
};

#endif

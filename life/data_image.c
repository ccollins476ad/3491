#include "life/image.h"
#include "life/data.h"

struct image_t data_images[IMAGE_ID_CNT] = {
    [IMAGE_ID_PP_STRAIGHT] = {
        .filename = "pp-straight.pcx",
        .xscale = 1.0,
        .yscale = 1.0,
    },
    [IMAGE_ID_GE_STRAIGHT] = {
        .filename = "ge-straight.pcx",
        .xscale = 2.0,
        .yscale = 2.0,
    },
};

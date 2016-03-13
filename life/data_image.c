#include "life/image.h"
#include "life/data.h"

struct image_t data_images[IMAGE_ID_CNT] = {
    [IMAGE_ID_PP_STRAIGHT] = {
        .filename = "pp-straight.pcx",
        .xscale = 8.0,
        .yscale = 8.0,
    },
};

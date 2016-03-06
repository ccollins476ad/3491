#ifndef H_FADER_
#define H_FADER_

#include "gfx/gfx.h"

struct fader_t {
    struct color_info_t color_info;
    int delta;
};

void fader_update(struct fader_t *fader);

#endif

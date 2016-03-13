#include <assert.h>
#include "gen/res.h"
#include "gfx/balleg.h"
#include "gfx/smart_resize.h"
#include "life/image.h"

static struct BITMAP *
image_load_bmp(char *filename)
{
    struct BITMAP *bmp;
    char *path;

    path = res_bmp_path(filename);
    bmp = load_bitmap(path, NULL);
    return bmp;
}

struct BITMAP *
image_bmp(struct image_t *image)
{
    struct BITMAP *unscaled;

    if (image->bmp == NULL) {
        assert(image->filename != NULL);

        unscaled = image_load_bmp(image->filename);
        assert(unscaled != NULL);

        if (image->xscale == 1.0 && image->yscale == 1.0) {
            image->bmp = unscaled;
        } else {
            image->bmp = smart_resize(unscaled,
                                      unscaled->w * image->xscale,
                                      unscaled->h * image->yscale);
            assert(image->bmp != NULL);

            destroy_bitmap(unscaled);
        }
    }
    
    return image->bmp;
}

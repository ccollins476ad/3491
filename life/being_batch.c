#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include "gen/gen_num.h"
#include "life/hit_box.h"
#include "life/vision.h"
#include "life/world.h"
#include "life/being_defs.h"
#include "life/being.h"

#define BEING_COLLISION_MAX_PUSH            (1 * WORLD_FACTOR)

struct being_t *
being_batch_find(unsigned int id, being_flags_t flags_any)
{
    struct being_t *being;
    int max_idx;
    int i;

    max_idx = being_max_idx();
    for (i = 0; i < max_idx; ++i) {
        being = being_at(i);
        if (being != NULL &&
            (id == ENTITY_ID_NONE || id == being->entity.id) &&
            (flags_any == 0 || flags_any & being->flags)) {

            return being;
        }
    }

    return NULL;
}

void
being_batch_update(void)
{
    struct being_t *being;
    int max_idx;
    int i;

    max_idx = being_max_idx();
    for (i = 0; i < max_idx; ++i) {
        being = being_at(i);
        if (being != NULL) {
            being_update(being);
        }
    }
}

void
being_batch_draw(void)
{
    struct being_t *being;
    int max_idx;
    int i;

    max_idx = being_max_idx();
    for (i = 0; i < max_idx; ++i) {
        being = being_at(i);
        if (being != NULL && being->flags & BEING_F_VISIBLE) {
            being_draw(being);
        }
    }
}

static void
being_batch_set_visible_once(struct being_t *being, int originx, int originy)
{
    int being_centerx;
    int being_centery;
    int prev_visible;
    int rc;

    /* Player is always visible; dead beings are always visible. */
    if (being->flags & BEING_F_PLAYER || being->hit_points <= 0) {
        being->flags |= BEING_F_VISIBLE;
        return;
    }

    prev_visible = being->flags & BEING_F_VISIBLE;

    being_center(being, &being_centerx, &being_centery);
    rc = vision_can_see(originx, originy, being_centerx, being_centery);
    if (rc) {
        being->flags |= BEING_F_VISIBLE;
    } else {
        being->flags &= ~BEING_F_VISIBLE;
    }

#if 0
    if ((being->flags & BEING_F_VISIBLE) ^ prev_visible) {
        being_ai_sig_visible_change(being);
    }
#endif
}

void
being_batch_set_visible(int originx, int originy)
{
    struct being_t *being;

    SLAB_FOREACH(&being_data.being_slab, being) {
        being_batch_set_visible_once(being, originx, originy);
    }
}


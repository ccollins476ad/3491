#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "gen/container.h"
#include "life/being.h"
#include "life/blast.h"
#include "life/data.h"
#include "life/object.h"
#include "life/world.h"

#define OBJECT_MAX  1024

struct object_data_t object_data;

static void
object_apply_opaque_raw(int x, int y, int opaque)
{
    terr_scape_set_opaque(x / WORLD_TILE_DIM, y / WORLD_TILE_DIM, opaque);
}

static void
object_apply_opaque(struct object_t *object)
{
    int opaque;

    opaque = object->properties->flags & OBJECT_PROPERTIES_F_OPAQUE;
    object_apply_opaque_raw(object->x, object->y, opaque);
}

struct object_t *
object_add(struct object_properties_t *properties, int x, int y)
{
    struct object_t *object;

    object = slab_insert(&object_data.slab);
    if (object == NULL) {
        /* XXX: Stats. */
        return NULL;
    }

    memset(object, 0, sizeof *object);
    object->entity.id = entity_next_id();
    object->entity.type = ENTITY_TYPE_OBJECT;

    object->properties = properties;
    object->x = x;
    object->y = y;
    object->hit_points = properties->defense.hit_points;

    object_apply_opaque(object);

    return object;
}

static void
object_erase(struct object_t *object)
{
    object_apply_opaque_raw(object->x, object->y, 0);
    slab_erase_elem(&object_data.slab, object);
}

void
object_clear(void)
{
    struct object_t *object;

    SLAB_FOREACH(&object_data.slab, object) {
        object->flags |= OBJECT_F_TRASH;
    }
}

void
object_center(struct object_t *object, int *centerx, int *centery)
{
    /* Assume the first hit box is centered. */
    hit_box_desc_center(object->properties->hit_boxes, centerx, centery);
    *centerx *= WORLD_FACTOR;
    *centery *= WORLD_FACTOR;

    *centerx += object->x;
    *centery += object->y;
}

static void
object_process_effect(struct object_t *object, struct being_t *user,
                      int effect)
{
    struct kv_elem_t *elem;
    struct kv_elem_t *kv;

    kv = object->properties->kv;

    switch (effect) {
    case OBJECT_EFFECT_CHANGE_TYPE:
        elem = kv_get_assert(kv, OBJECT_K_TYPE_ON_USE);
        object->properties = elem->val.v;
        object_apply_opaque(object);
        break;

    case OBJECT_EFFECT_TELEPORT:
        being_teleport(user,
                       KV_GET_VAL_DFLT(kv, OBJECT_K_LEVEL, v, NULL),
                       KV_GET_VAL_ASSERT(kv, OBJECT_K_X, i),
                       KV_GET_VAL_ASSERT(kv, OBJECT_K_Y, i));
        break;

    default:
        assert(0);
        break;
    }
}

static void
object_activate(struct object_t *object, struct being_t *user)
{
    int *effects;
    int i;

    effects = object->properties->effects;
    if (effects == NULL) {
        return;
    }

    for (i = 0; effects[i] != -1; i++) {
        object_process_effect(object, user, effects[i]);
    }
}

void
object_use(struct object_t *object)
{
    object_activate(object, NULL);
}

void
object_die(struct object_t *object)
{
    struct kv_elem_t *kv;
    int *effects;
    int i;

    effects = object->properties->effects;
    if (effects == NULL) {
        return;
    }
    kv = object->properties->kv;
    if (kv == NULL) {
        return;
    }

    object->flags |= OBJECT_F_TRASH;

    for (i = 0; effects[i] != -1; i++) {
        if (effects[i] == OBJECT_EFFECT_BLAST) {
            for (i = 0; kv[i].key != -1; i++) {
                if (kv[i].key == OBJECT_K_BLAST_PROPERTIES) {
                    blast_apply(kv[i].val.v, object->x, object->y, 0);
                    return;
                }
            }
            return;
        }
    }
}

static void
object_build_hit_box(struct object_t *object, struct hit_box_desc_t *desc,
                     struct hit_box_t *hit_box)
{
    hit_box_from_desc(hit_box, desc, object->x, object->y);
    hit_box->owner = &object->entity;
}

void
object_process_hit_boxes(void)
{
    struct hit_box_intersection_t *intersection;
    struct object_t *object;
    struct being_t *user;
    int i;

    hit_box_process_tch();
    for (i = 0; i < hit_box_intersections.size; ++i) {
        intersection = pslab_at(&hit_box_intersections, i);
        if (intersection->a->owner->type == ENTITY_TYPE_BEING) {
            user = entity_to_being(intersection->a->owner);
            object = entity_to_object(intersection->b->owner);
            object_activate(object, user);
        }
    }
}

void
object_update_all(void)
{
    struct hit_box_desc_t *desc;
    struct hit_box_t hit_box;
    struct object_t *object;

    SLAB_FOREACH(&object_data.slab, object) {
        if (object->flags & OBJECT_F_TRASH) {
            object_erase(object);
        } else {
            HIT_BOX_DESCS_FOREACH(object->properties->hit_boxes, desc) {
                object_build_hit_box(object, desc, &hit_box);
                hit_box_add(&hit_box);
            }
        }
    }
}

void
object_draw_all(void)
{
    struct object_t *object;

    SLAB_FOREACH(&object_data.slab, object) {
        world_view_draw_icon(&object->properties->icon, NULL,
                             object->x, object->y);
    }
}

void
object_init(void)
{
    int rc;

    slab_destroy(&object_data.slab);

    rc = slab_create(&object_data.slab, sizeof (struct object_t), OBJECT_MAX);
    assert (rc == 0);
}



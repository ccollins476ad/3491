#include <stddef.h>
#include <assert.h>
#include "gen/container.h"
#include "life/being_defs.h"
#include "life/being.h"
#include "life/world.h"
#include "life/level.h"
#include "life/blast.h"
//#include "life/arena.h"
#include "life/scroll.h"
#include "life/event.h"

#define EVENT_MAX       256

struct {
    struct slab_t slab;
    struct event_t *list;
} event_data;

static void
event_insert(struct event_t *event)
{
    struct event_t *prev;
    struct event_t *cur;

    if (event_data.list == NULL) {
        event->next = NULL;
        event_data.list = event;
        return;
    }

    prev = NULL;
    for (cur = event_data.list; cur != NULL; cur = cur->next) {
        if (event->exp_at <= cur->exp_at) {
            break;
        }
        prev = cur;
    }

    if (prev == NULL) {
        event->next = event_data.list;
        event_data.list = event;
    } else {
        event->next = prev->next; 
        prev->next = event;
    }
}

struct event_t *
event_add(int type, int exp_at)
{
    struct event_t *event;

    assert(type >= 0 && type < EVENT_TYPE_MAX);

    event = slab_insert(&event_data.slab);
    if (event == NULL) {
        return NULL;
    }

    event->type = type;
    event->exp_at = exp_at;
    event_insert(event);

    return event;
}

static void
event_execute_trash_being(struct event_t *event)
{
    struct being_t *being;

    assert(event->type == EVENT_TYPE_TRASH_BEING);

    being = being_batch_find(event->trash_being.being_id, 0);
    if (being != NULL) {
        being->flags |= BEING_F_TRASH;
    }
}

static void
event_execute_teleport_being(struct event_t *event)
{
    struct event_teleport_being_t *etb;
    struct being_t *being;

    assert(event->type == EVENT_TYPE_TELEPORT_BEING);
    etb = &event->teleport_being;

    being = being_batch_find(event->trash_being.being_id, 0);
    if (being == NULL) {
        return;
    }

    /* Load new level before setting being's position. */
    if (etb->level != NULL) {
        if (being->flags & BEING_F_PLAYER) {
            //arena_save();
            //arena_load(etb->level);
        } else {
            // XXX
        }
    }
    being->phys.x = etb->x;
    being->phys.y = etb->y;

    if (being->flags & BEING_F_PLAYER) {
        scroll_update();
    }
}

static void
event_execute_blast(struct event_t *event)
{
    struct event_blast_t *blast;

    assert(event->type == EVENT_TYPE_BLAST);
    blast = &event->blast;

    blast_apply_attack(blast->properties, blast->x, blast->y,
                       blast->shrapnel_owner_id);
}

static void
event_execute_fade_being(struct event_t *event)
{
    struct event_fade_being_t *efb;
    struct being_t *being;

    assert(event->type == EVENT_TYPE_FADE_BEING);
    efb = &event->fade_being;

    being = being_batch_find(event->fade_being.being_id, 0);
    if (being != NULL) {
        being->fader = *efb->fader;
    }
}

typedef void event_dispatch_fn_t(struct event_t *event);
static event_dispatch_fn_t* event_dispatch_table[EVENT_TYPE_MAX] = {
    [EVENT_TYPE_TRASH_BEING] = event_execute_trash_being,
    [EVENT_TYPE_FADE_BEING] = event_execute_fade_being,
    [EVENT_TYPE_TELEPORT_BEING] = event_execute_teleport_being,
    [EVENT_TYPE_BLAST] = event_execute_blast,
};

static void
event_execute(struct event_t *event)
{
    event_dispatch_fn_t *fn;

    assert(event->type >= 0 && event->type < EVENT_TYPE_MAX);

    fn = event_dispatch_table[event->type];
    fn(event);
}

void
event_update(void)
{
    while (event_data.list != NULL && world_tick >= event_data.list->exp_at) {
        event_execute(event_data.list);
        slab_erase_elem(&event_data.slab, event_data.list);
        event_data.list = event_data.list->next;
    }
}

int
event_init(void)
{
    int rc;

    rc = slab_create(&event_data.slab, sizeof (struct event_t),
                     EVENT_MAX);
    if (rc != 0) {
        return rc;
    }

    return 0;
}


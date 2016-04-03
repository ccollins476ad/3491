#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "life/data.h"
#include "life/stats.h"
#include "life/being_defs.h"
#include "life/being.h"

#define BEING_MAX_BEINGS    100
#define BEING_MAX_INTEREST  256
#define BEING_MAX_TRAITS    (BEING_MAX_BEINGS * 16)

struct being_data_t being_data;

/*****************************************************************************
 * @being interest                                                           *
 *****************************************************************************/

struct being_interest_entry_t *
being_interest_find(struct being_t *being,
                    struct entity_t *entity,
                    int purpose,
                    struct being_interest_entry_t *sought,
                    struct being_interest_entry_t **prev_out)
{
    struct being_interest_entry_t *entry;
    struct being_interest_entry_t *prev;

    prev = NULL;
    entry = being->interest_list;
    while (entry != NULL) {
        if ((purpose == BEING_PURPOSE_NONE  || purpose == entry->purpose)   &&
            (entity  == NULL                || entity == entry->entity)     &&
            (sought  == NULL                || sought == entry)) {

            break;
        }

        prev = entry;
        entry = entry->next;
    }

    if (prev_out != NULL) {
        *prev_out = prev;
    }

    return entry;
}

struct being_interest_entry_t *
being_interest_add(struct being_t *being)
{
    struct being_interest_entry_t *entry;

    entry = slab_insert(&being_data.interest_slab);
    if (entry != NULL) {
        entry->next = being->interest_list;
        being->interest_list = entry;
    }
    return entry;
}

void
being_interest_remove(struct being_t *being,
                      struct being_interest_entry_t *entry,
                      struct being_interest_entry_t *prev)
{
    if (prev == NULL) {
        /* Head of list. */
        being->interest_list = entry->next;
    } else {
        prev->next = entry->next;
    }

    slab_erase_elem(&being_data.interest_slab, entry);
}

/*****************************************************************************
 * @being traits                                                             *
 *****************************************************************************/

struct being_traits_entry_t *
being_traits_find(struct being_t *being, int type,
                  struct being_traits_entry_t **prev_out)
{
    struct being_traits_entry_t *entry;
    struct being_traits_entry_t *prev;

    prev = NULL;
    entry = being->traits_list;
    while (entry != NULL) {
        if (type == entry->type) {
            break;
        }

        prev = entry;
        entry = entry->next;
    }

    if (prev_out != NULL) {
        *prev_out = prev;
    }

    return entry;
}

int
being_traits_find_dflt(struct being_t *being, int type, int dflt)
{
    struct being_traits_entry_t *entry;

    entry = being_traits_find(being, type, NULL);
    if (entry != NULL) {
        return entry->val;
    } else {
        return dflt;
    }
}

struct being_traits_entry_t *
being_traits_add(struct being_t *being)
{
    struct being_traits_entry_t *entry;

    entry = slab_insert(&being_data.traits_slab);
    if (entry == NULL) {
        stats_data.being.traits_oom++;
        return NULL;
    }

    entry->next = being->traits_list;
    being->traits_list = entry;
    return entry;
}

struct being_traits_entry_t *
being_traits_ensure(struct being_t *being, int type)
{
    struct being_traits_entry_t *entry;

    entry = being_traits_find(being, type, NULL);
    if (entry != NULL) {
        return entry;
    }

    entry = being_traits_add(being);
    if (entry != NULL) {
        entry->type = type;
        entry->val = 0;
    }

    return entry;
}

int
being_traits_replace(struct being_t *being, int type, int val)
{
    struct being_traits_entry_t *entry;

    entry = being_traits_ensure(being, type);
    if (entry == NULL) {
        return -1;
    }

    entry->val = val;
    return 0;
}

void
being_traits_remove(struct being_t *being,
                    struct being_traits_entry_t *entry,
                    struct being_traits_entry_t *prev)
{
    if (prev == NULL) {
        /* Head of list. */
        being->traits_list = entry->next;
    } else {
        prev->next = entry->next;
    }

    slab_erase_elem(&being_data.traits_slab, entry);
}

void
being_traits_remove_type(struct being_t *being, int type)
{
    struct being_traits_entry_t *entry;
    struct being_traits_entry_t *prev;

    entry = being_traits_find(being, type, &prev);
    if (entry != NULL) {
        being_traits_remove(being, entry, prev);
    }
}

int
being_traits_remove_dflt(struct being_t *being, int type, int dflt)
{
    struct being_traits_entry_t *entry;
    struct being_traits_entry_t *prev;
    int val;

    entry = being_traits_find(being, type, &prev);
    if (entry != NULL) {
        val = entry->val;
        being_traits_remove(being, entry, prev);
    } else {
        val = dflt;
    }

    return val;
}

/*****************************************************************************
 * @being                                                                    *
 *****************************************************************************/

int
being_max_idx(void)
{
    return slab_max(&being_data.being_slab);
}

struct being_t *
being_at(int idx)
{
    struct being_t *being;

    being = slab_at(&being_data.being_slab, idx);
    return being;
}

void
being_erase(struct being_t *being)
{
    struct being_interest_entry_t *interest_next;
    struct being_interest_entry_t *interest_cur;
    struct being_traits_entry_t *traits_next;
    struct being_traits_entry_t *traits_cur;

    entity_trashed(&being->entity);

    interest_cur = being->interest_list;
    while (interest_cur != NULL) {
        interest_next = interest_cur->next;
        slab_erase_elem(&being_data.interest_slab, interest_cur);
        interest_cur = interest_next;
    }

    traits_cur = being->traits_list;
    while (traits_cur != NULL) {
        traits_next = traits_cur->next;
        slab_erase_elem(&being_data.traits_slab, traits_cur);
        traits_cur = traits_next;
    }

    //ai_ctxt_free(&being->ai_ctxt);

    slab_erase_elem(&being_data.being_slab, being);
}

struct being_t *
being_add(int type)
{
    struct being_t *being;

    assert(type >= 0 && type < BEING_TYPE_MAX);

    being = slab_insert(&being_data.being_slab);
    if (being == NULL) {
        return NULL;
    }

    memset(being, 0, sizeof *being);
    being->entity.id = entity_next_id();
    being->entity.type = ENTITY_TYPE_BEING;
    being->properties = data_being_properties + type;
    being->gun.properties = data_gun_properties + GUN_TYPE_NULL;
    being->hit_points = being->properties->defense.hit_points;
    being->flags = being->properties->flags;
    fsm_ctxt_create(&being->fsm_ctxt);

    being_set_fsm_state(being, FSM_STATE_STAND);

    //being->ai_ctxt.next_ffi_state = AI_CTXT_NEXT_FFI_STATE_STALE;

    being->log_ctxt.fsm = 1;
    being->log_ctxt.ai = 1;

    return being;
}

void
being_init(void)
{
    int rc;

    slab_destroy(&being_data.being_slab);
    slab_destroy(&being_data.interest_slab);
    slab_destroy(&being_data.traits_slab);

    rc = slab_create(&being_data.being_slab, sizeof (struct being_t),
                     BEING_MAX_BEINGS);
    assert(rc == 0);

    rc = slab_create(&being_data.interest_slab,
                     sizeof (struct being_interest_entry_t),
                     BEING_MAX_INTEREST);
    assert(rc == 0);

    rc = slab_create(&being_data.traits_slab,
                     sizeof (struct being_traits_entry_t),
                     BEING_MAX_TRAITS);
    assert(rc == 0);
}

void
being_clear(void)
{
    struct being_t *being;

    SLAB_FOREACH(&being_data.being_slab, being) {
        if (!(being->flags & BEING_F_PLAYER)) {
            being->flags |= BEING_F_TRASH;
        }
    }
}


#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include "gen/container.h"
#include "life/carnage.h"
#include "life/being.h"
#include "life/object.h"
#include "life/entity.h"

#define ENTITY_SIGNAL_ENTRY_MAX     256

struct {
    unsigned int next_id;
    struct slab_t slab;
} entity_data;

struct entity_signal_entry_t {
    struct entity_signal_entry_t *next;
    struct entity_t *receiver;
    uint8_t signal;
};

unsigned int
entity_next_id(void)
{
    unsigned int id;

    id = entity_data.next_id++;
    if (entity_data.next_id >= ENTITY_ID_MAX) {
        entity_data.next_id = 0;
    }

    return id;
}

struct being_t *
entity_to_being(struct entity_t *entity)
{
    assert(entity->type == ENTITY_TYPE_BEING);
    return (void *)entity;
}

struct bullet_t *
entity_to_bullet(struct entity_t *entity)
{
    assert(entity->type == ENTITY_TYPE_BULLET);
    return (void *)entity;
}

struct object_t *
entity_to_object(struct entity_t *entity)
{
    assert(entity->type == ENTITY_TYPE_OBJECT);
    return (void *)entity;
}

struct proxy_attack_t *
entity_to_proxy_attack(struct entity_t *entity)
{
    assert(entity->type == ENTITY_TYPE_PROXY_ATTACK);
    return (void *)entity;
}

struct carnage_t *
entity_to_carnage(struct entity_t *entity)
{
    assert(entity->type == ENTITY_TYPE_CARNAGE);
    return (void *)entity;
}

static struct entity_signal_entry_t *
entity_signal_find(struct entity_signal_entry_t *list,
                   struct entity_t *receiver)
{
    struct entity_signal_entry_t *entry;

    for (entry = list; entry != NULL; entry = entry->next) {
        if (entry->receiver == receiver) {
            return entry;
        }
    }

    return NULL;
}

int
entity_add_signal(struct entity_t *sender, struct entity_t *receiver,
                  uint8_t signal)
{
    struct entity_signal_entry_t *entry_recip_new;
    struct entity_signal_entry_t *entry_main_new;
    struct entity_signal_entry_t *entry_recip;
    struct entity_signal_entry_t *entry_main;

    entry_main_new = NULL;
    entry_recip_new = NULL;

    /* Both entities need to signal each other.  x --> y is what is
     * being requested.  y --> x needs to happen to ensure that x doesn't
     * try to signal y after y dies.
     */
    entry_main = entity_signal_find(sender->signal_list, receiver);
    if (entry_main == NULL) {
        entry_main_new = slab_insert(&entity_data.slab);
        if (entry_main_new == NULL) {
            goto err;
        }
        entry_main = entry_main_new;
        memset(entry_main, 0, sizeof *entry_main);
        entry_main->receiver = receiver;
        entry_main->next = sender->signal_list;
        sender->signal_list = entry_main;
    }

    entry_recip = entity_signal_find(receiver->signal_list, sender);
    if (entry_recip == NULL) {
        entry_recip_new = slab_insert(&entity_data.slab);
        if (entry_recip_new == NULL) {
            goto err;
        }
        entry_recip = entry_recip_new;
        memset(entry_recip, 0, sizeof *entry_recip);
        entry_recip->receiver = sender;
        entry_recip->next = receiver->signal_list;
        receiver->signal_list = entry_recip;
    }

    entry_main->signal |= signal;
    entry_recip->signal |= ENTITY_SIGNAL_TRASH;

    return 0;

err:
    slab_erase_elem(&entity_data.slab, entry_main_new);
    slab_erase_elem(&entity_data.slab, entry_recip_new);

    return -1;
}

static void
entity_erase_signal(struct entity_t *entity, struct entity_t *receiver)
{
    struct entity_signal_entry_t *entry;
    struct entity_signal_entry_t *prev;

    prev = NULL;
    entry = entity->signal_list;
    while (entry != NULL) {
        if (entry->receiver == receiver) {
            if (prev == NULL) {
                /* Head of list. */
                entity->signal_list = entry->next;
            } else {
                prev->next = entry->next;
            }
            slab_erase_elem(&entity_data.slab, entry);
        }
        prev = entry;
        entry = entry->next;
    }
}

static void
entity_send_signal(struct entity_t *sender,
                   struct entity_signal_entry_t *entry, uint8_t signal)
{
    struct carnage_t *carnage;
    struct being_t *being;

    switch (entry->receiver->type) {
    case ENTITY_TYPE_BEING:
        being = entity_to_being(entry->receiver);
        being_signal(being, sender, signal);
        break;

    case ENTITY_TYPE_CARNAGE:
        carnage = entity_to_carnage(entry->receiver);
        carnage_signal(carnage, sender, signal);
        break;

    default:
        break;
    }
}

void
entity_trashed(struct entity_t *entity)
{
    struct entity_signal_entry_t *entry;
    struct entity_signal_entry_t *next;

    entry = entity->signal_list;
    while (entry != NULL) {
        next = entry->next;

        if (entry->signal & ENTITY_SIGNAL_TRASH) {
            entity_send_signal(entity, entry, ENTITY_SIGNAL_TRASH);
        }

        /* Tell the receiver not to signal the just-trashed entity. */
        entity_erase_signal(entry->receiver, entity);

        slab_erase_elem(&entity_data.slab, entry);
        entry = next;
    }

    entity->signal_list = NULL;
}

void
entity_signal(struct entity_t *sender, uint8_t signal)
{
    struct entity_signal_entry_t *entry;

    assert(!(signal & ENTITY_SIGNAL_TRASH));

    /* Send the specified signal to all entities listening for it. */
    for (entry = sender->signal_list; entry != NULL; entry = entry->next) {
        if (entry->signal & signal) {
            entity_send_signal(sender, entry, signal);
        }
    }
}

int
entity_init(void)
{
    int rc;

    slab_destroy(&entity_data.slab);
    entity_data.next_id = 0;

    rc = slab_create(&entity_data.slab, sizeof (struct entity_signal_entry_t),
                     ENTITY_SIGNAL_ENTRY_MAX);
    return rc;
}


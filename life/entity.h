#ifndef ENTITY_H_
#define ENTITY_H_

#include <inttypes.h>

struct being_t;
struct bullet_t;
struct object_t;
struct proxy_attack_t;

#define ENTITY_ID_NONE          UINT32_MAX
#define ENTITY_ID_MAX           UINT32_MAX

#define ENTITY_TYPE_BEING           0
#define ENTITY_TYPE_BULLET          1
#define ENTITY_TYPE_OBJECT          2
#define ENTITY_TYPE_PROXY_ATTACK    3
#define ENTITY_TYPE_CARNAGE         4
#define ENTITY_TYPE_MAX             5

#define ENTITY_SIGNAL_TRASH     0x01

struct entity_signal_entry_t;

struct entity_t {
    unsigned int id;
    int type;
    struct entity_signal_entry_t *signal_list;
};

unsigned int entity_next_id(void);

struct being_t *entity_to_being(struct entity_t *entity);
struct bullet_t *entity_to_bullet(struct entity_t *entity);
struct object_t *entity_to_object(struct entity_t *entity);
struct proxy_attack_t *entity_to_proxy_attack(struct entity_t *entity);
struct carnage_t *entity_to_carnage(struct entity_t *entity);

int entity_add_signal(struct entity_t *sender, struct entity_t *receiver,
                      uint8_t signal);
void entity_erase_listener(struct entity_t *receiver);
void entity_trashed(struct entity_t *entity);
void entity_signal(struct entity_t *sender, uint8_t signal);
int entity_init(void);

/* Util. */
int entity_util_hostile(struct entity_t *entity);

#endif


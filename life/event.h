#ifndef EVENT_H_
#define EVENT_H_

#define EVENT_TYPE_TRASH_BEING          0
#define EVENT_TYPE_FADE_BEING           1
#define EVENT_TYPE_TELEPORT_BEING       2
#define EVENT_TYPE_BLAST                3
#define EVENT_TYPE_MAX                  4

struct event_trash_being_t {
    unsigned int being_id;
};

struct event_fade_being_t {
    unsigned int being_id;
    struct fader_t *fader;
};

struct event_teleport_being_t {
    unsigned int being_id;
    struct level_t *level;
    int x;
    int y;
};

struct event_blast_t {
    struct blast_properties_t *properties;
    int shrapnel_owner_id;
    int x;
    int y;
};

struct event_t {
    struct event_t *next;
    int type;
    unsigned int exp_at;

    union {
        struct event_trash_being_t trash_being;
        struct event_fade_being_t fade_being;
        struct event_teleport_being_t teleport_being;
        struct event_blast_t blast;
    };
};

struct event_t *event_add(int type, int exp_at);
void event_update(void);
int event_init(void);

#endif


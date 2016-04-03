#ifndef H_PLAYER_
#define H_PLAYER_

struct player_t {
    struct being_t *being;
};

extern struct player_t player;

void player_update_visible(void);

#endif


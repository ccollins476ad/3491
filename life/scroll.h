#ifndef H_SCROLL_
#define H_SCROLL_

#define SCROLL_MODE_PLAYER  0
#define SCROLL_MODE_FREE    1

struct scroll_data_t {
    int mode;
    int centerx;
    int centery;
};

void scroll_update(void);

extern struct scroll_data_t scroll_data;

#endif


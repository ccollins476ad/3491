#ifndef H_TICKER_
#define H_TICKER_

struct ticker_t {
    int rate;
    int cur;
};

int ticker_update(struct ticker_t *ticker, int amount);

#endif


#include "gen/ticker.h"

int
ticker_update(struct ticker_t *ticker, int amount)
{
    int ticks;

    ticker->cur += amount;
    ticks = ticker->cur / ticker->rate;
    ticker->cur %= ticker->rate;

    return ticks;
}    

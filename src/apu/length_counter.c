#include "length_counter.h"

void length_counter_tick(struct length_counter *lc, bool *output)
{
    if (lc->enabled && lc->counter > 0) {
        if (--lc->counter == 0) {
            /* Disable output */
            *output = false;
        }
    }
}

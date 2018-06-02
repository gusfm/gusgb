#ifndef LENGTH_COUNTER_H
#define LENGTH_COUNTER_H

#include <stdbool.h>

struct length_counter {
    unsigned int counter;
    bool enabled;
};

void length_counter_tick(struct length_counter *lc, bool *output);

#endif /* LENGTH_COUNTER_H */

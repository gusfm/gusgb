#ifndef LENGTH_COUNTER_H
#define LENGTH_COUNTER_H

#include <stdbool.h>
#include <stdint.h>

struct length_counter {
    unsigned int counter;
    uint8_t enabled;
};

void length_counter_tick(struct length_counter *lc, bool *output);

#endif /* LENGTH_COUNTER_H */

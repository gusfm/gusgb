#ifndef __AI_H__
#define __AI_H__

#include <stdint.h>

#define NUM_INPUTS (160 * 144 / 4)
#define NUM_OUTPUTS (6)

typedef struct {
    uint8_t screen[NUM_INPUTS];
    uint8_t keys;
} training_set_t;

#endif /* __AI_H__ */

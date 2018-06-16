#ifndef VOLUME_ENVELOPE
#define VOLUME_ENVELOPE

#include <stdbool.h>
#include <stdint.h>

struct volume_envelope {
    unsigned int volume;
    unsigned int direction;
    unsigned int period;
    int count;
};

uint8_t volume_envelope_read(struct volume_envelope *env);
void volume_envelope_write(struct volume_envelope *env, uint8_t val,
                           bool *enabled);
void volume_envelope_tick(struct volume_envelope *env);

#endif /* VOLUME_ENVELOPE */

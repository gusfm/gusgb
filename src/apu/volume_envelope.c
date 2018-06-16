#include "volume_envelope.h"

uint8_t volume_envelope_read(struct volume_envelope *env)
{
    return (env->volume << 4) | (env->direction << 3) | env->period;
}

void volume_envelope_write(struct volume_envelope *env, uint8_t val,
                           bool *enabled)
{
    env->volume = val >> 4;
    env->direction = (val >> 3) & 1;
    env->period = val & 0x7;
    env->count = env->period;
    /* Check if DAC is disabled */
    if (!(val & 0xf8)) {
        /* Disable channel output */
        *enabled = false;
    }
}

void volume_envelope_tick(struct volume_envelope *env)
{
    if (env->period > 0 && --env->count <= 0) {
        env->count = env->period;
        if (env->direction) {
            if (env->volume < 0xf) {
                env->volume++;
            }
        } else {
            if (env->volume > 0) {
                env->volume--;
            }
        }
    }
}

#ifndef VOLUME_ENVELOPE
#define VOLUME_ENVELOPE

struct volume_envelope {
    unsigned int volume;
    unsigned int direction;
    unsigned int period;
    int count;
};

#endif /* VOLUME_ENVELOPE */

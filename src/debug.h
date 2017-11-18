#ifndef DEBUG_H
#define DEBUG_H

#include <stdlib.h>

#ifdef DEBUG
#define printd(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define printd(fmt, ...) ((void)0)
#endif

#define warn(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__);

#define error(fmt, ...)                            \
    {                                              \
        warn("%s: " fmt, __func__, ##__VA_ARGS__); \
        abort();                                   \
    }

#endif /* DEBUG_H */

#include "random.h"
#include <stdlib.h>
#include <time.h>

void rand_start(void)
{
    srand((unsigned int)time(NULL));
}

/* Returns a random double in the range [0,1]. */
inline double rand0(void)
{
    return ((double)rand() / RAND_MAX);
}

/* Returns a random double in the range [-1,1]. */
inline double rand1(void)
{
    return rand0() * 2.0 - 1.0;
}

/* Returns a random double in the range [0.5,1.5]. */
inline double rand3(void)
{
    return rand0() + 0.5;
}

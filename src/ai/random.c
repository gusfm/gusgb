#include "random.h"
#include <stdlib.h>
#include <time.h>

void rand_start(void)
{
    srand((unsigned int)time(NULL));
}

/* Returns a random double in the range -1 <= n <= 1. */
double rand_clamp(void)
{
    return ((double)rand() / RAND_MAX) * 2.0 - 1.0;
}

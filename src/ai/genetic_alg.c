#include "genetic_alg.h"
#include <stdlib.h>
#include "random.h"

#define CROSSOVER_PROBABILITY (50)
#define MUTATION_RATE (30) /* Mutation rate percent. */
#define MAX_PERTUBATION (0.5)

static unsigned int ga_calc_crossover_point(unsigned int chromo_length)
{
    return (unsigned int)rand() % chromo_length;
}

static double ga_mutate(double gene)
{
    /* Should we perturb this weight? */
    if (((unsigned int)rand() % 100) < MUTATION_RATE) {
        /* Add or subtract a small value to the weight. */
        gene += (rand_clamp() * MAX_PERTUBATION);
    }
    return gene;
}

void ga_crossover1(unsigned int chromo_length, const double *mom,
                   const double *dad, double *child1, double *child2)
{
    unsigned int i;
    /* Get crossover point. */
    unsigned int cp = ga_calc_crossover_point(chromo_length);
    for (i = 0; i < cp; ++i) {
        child1[i] = ga_mutate(mom[i]);
        child2[i] = ga_mutate(dad[i]);
    }
    for (i = cp; i < chromo_length; ++i) {
        child1[i] = ga_mutate(dad[i]);
        child2[i] = ga_mutate(mom[i]);
    }
}

void ga_crossover2(unsigned int chromo_length, const double *mom,
                   const double *dad, double *child1, double *child2)
{
    for (unsigned int i = 0; i < chromo_length; ++i) {
        if (rand() % 100 < CROSSOVER_PROBABILITY) {
            child1[i] = ga_mutate(mom[i]);
            child2[i] = ga_mutate(dad[i]);
        } else {
            child1[i] = ga_mutate(dad[i]);
            child2[i] = ga_mutate(mom[i]);
        }
    }
}

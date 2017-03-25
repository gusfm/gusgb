#include "genetic_alg.h"
#include <stdlib.h>
#include "random.h"

#define CROSSOVER_PROBABILITY (50)
#define MUTATION_RATE (30) /* Mutation rate percent. */
#define MAX_PERTUBATION (0.3)

static double ga_mutate(double gene)
{
    /* Should we perturb this weight? */
    unsigned int number = (unsigned int)rand() % 100;
    if (number < MUTATION_RATE) {
        if (number < 10) {
            /* Add small value to the weight. */
            gene += (rand0() * MAX_PERTUBATION);
        } else if (number < 20) {
            /* Subtract small value to the weight. */
            gene -= (rand0() * MAX_PERTUBATION);
        } else if (number < 25) {
            /* Sale the weight. */
            gene *= rand3();
        } else {
            /* Replace weight by a random value. */
            gene = rand1();
        }
    }
    return gene;
}

void ga_crossover1(unsigned int chromo_length, const double *mom,
                   const double *dad, double *child1, double *child2)
{
    unsigned int i;
    /* Get crossover point. */
    unsigned int cp = (unsigned int)rand() % chromo_length;
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

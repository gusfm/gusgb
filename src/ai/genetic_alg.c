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

static void ga_crossover1(crossover_t *cross)
{
    unsigned int i;
    /* Get crossover point. */
    unsigned int cp = (unsigned int)rand() % cross->chromo_length;
    for (i = 0; i < cp; ++i) {
        cross->child1[i] = ga_mutate(cross->mom[i]);
        cross->child2[i] = ga_mutate(cross->dad[i]);
    }
    for (i = cp; i < cross->chromo_length; ++i) {
        cross->child1[i] = ga_mutate(cross->dad[i]);
        cross->child2[i] = ga_mutate(cross->mom[i]);
    }
}

static void ga_crossover2(crossover_t *cross)
{
    for (unsigned int i = 0; i < cross->chromo_length; ++i) {
        if (rand() % 100 < CROSSOVER_PROBABILITY) {
            cross->child1[i] = ga_mutate(cross->mom[i]);
            cross->child2[i] = ga_mutate(cross->dad[i]);
        } else {
            cross->child1[i] = ga_mutate(cross->dad[i]);
            cross->child2[i] = ga_mutate(cross->mom[i]);
        }
    }
}

void ga_crossover(crossover_t *cross)
{
    unsigned int number = (unsigned int)rand() % 100;
    if (number < 50) {
        ga_crossover1(cross);
    } else {
        ga_crossover2(cross);
    }
}

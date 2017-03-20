#include "population.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "genetic_alg.h"
#include "random.h"

population_t *population_create(unsigned int num_players,
                                unsigned int num_inputs,
                                unsigned int num_outputs)
{
    if (num_players == 0 || num_players & 1) {
        fprintf(stderr,
                "%s: num_players should be an even number greater than zero\n",
                __func__);
        return NULL;
    }
    population_t *pop = (population_t *)malloc(sizeof(population_t));
    pop->num_players = num_players;
    pop->players = (player_t **)malloc(sizeof(player_t) * num_players);
    rand_start();
    player_t *player;
    for (unsigned int i = 0; i < num_players; ++i) {
        player = player_create(num_inputs, num_outputs);
        pop->players[i] = player;
    }
    pop->num_chromo = player_get_chromosome_lenght(player);
    pop->mom_weights = (double *)malloc(sizeof(double) * pop->num_chromo);
    pop->dad_weights = (double *)malloc(sizeof(double) * pop->num_chromo);
    pop->child1_weights = (double *)malloc(sizeof(double) * pop->num_chromo);
    pop->child2_weights = (double *)malloc(sizeof(double) * pop->num_chromo);
    return pop;
}

void population_destroy(population_t *pop)
{
    for (unsigned int i = 0; i < pop->num_players; ++i) {
        player_destroy(pop->players[i]);
    }
    free(pop->players);
    free(pop->mom_weights);
    free(pop->dad_weights);
    free(pop->child1_weights);
    free(pop->child2_weights);
}

static int compare_sort(const void *first, const void *second)
{
    unsigned int fitness1 = player_get_fitness((player_t *)first);
    unsigned int fitness2 = player_get_fitness((player_t *)second);
    if (fitness1 < fitness2)
        return -1;
    if (fitness1 > fitness2)
        return 1;
    return 0;
}

void population_natural_selection(population_t *pop)
{
    /* Sort agents list. */
    qsort(pop->players, pop->num_players, sizeof(player_t *), compare_sort);
    /* Get half of population, and reproduce them. */
    unsigned int half_pop = pop->num_players / 2;
    for (unsigned int i = 0; i < half_pop; i += 2) {
        /* Get parent's weights. */
        player_get_chromosome(pop->players[i], pop->mom_weights,
                              pop->num_chromo);
        player_get_chromosome(pop->players[i + 1], pop->dad_weights,
                              pop->num_chromo);
        /* Crossover. */
        ga_crossover1(pop->num_chromo, pop->mom_weights, pop->dad_weights,
                      pop->child1_weights, pop->child2_weights);
        /* Save new child weights. */
        player_set_chromosome(pop->players[half_pop + i], pop->child1_weights,
                              pop->num_chromo);
        player_set_chromosome(pop->players[half_pop + i + 1],
                              pop->child2_weights, pop->num_chromo);
    }
}

player_t *population_get_player(population_t *pop, unsigned int player_id)
{
    assert(player_id < pop->num_players);
    return pop->players[player_id];
}
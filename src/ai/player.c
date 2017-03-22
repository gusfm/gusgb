#include "player.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_OUTPUTS 6
#define NUM_HIDDEN_LAYERS 3
#define NUM_NEURONS_PER_LAYER 20

player_t *player_create(unsigned int num_inputs, unsigned int num_outputs)
{
    player_t *p = (player_t *)malloc(sizeof(player_t));
    p->nn = neural_net_create(num_inputs, num_outputs, NUM_HIDDEN_LAYERS,
                              NUM_NEURONS_PER_LAYER);
    p->age = 0;
    return p;
}

void player_destroy(player_t *p)
{
    neural_net_destroy(p->nn);
}

unsigned int player_get_chromosome_lenght(player_t *p)
{
    return neural_net_get_num_weights(p->nn);
}

void player_get_chromosome(player_t *p, double *chromo_array,
                           unsigned int array_length)
{
    neural_net_get_weights(p->nn, chromo_array, array_length);
    p->age++;
}

void player_set_chromosome(player_t *p, double *chromo_array,
                           unsigned int array_length)
{
    neural_net_set_weights(p->nn, chromo_array, array_length);
    p->age = 0;
}

const double *player_output(player_t *p, const double *inputs)
{
    return neural_net_output(p->nn, inputs);
}

unsigned int player_get_fitness(player_t *p)
{
    return p->fitness;
}

void player_set_fitness(player_t *p, unsigned int fitness)
{
    p->fitness = fitness;
}

unsigned int player_get_age(player_t *p)
{
    return p->age;
}

void player_print(player_t *p)
{
    printf("Player: fitness: %u\n", p->fitness);
    neural_net_print(p->nn);
}

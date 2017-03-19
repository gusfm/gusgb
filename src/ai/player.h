#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "neural_net.h"

typedef struct {
    neural_net_t *nn;
    unsigned int fitness;
} player_t;

player_t *player_create(unsigned int num_inputs, unsigned int num_outputs);
void player_destroy(player_t *p);
unsigned int player_get_chromosome_lenght(player_t *p);
void player_get_chromosome(player_t *p, double *chromo_array,
                           unsigned int array_length);
void player_set_chromosome(player_t *p, double *chromo_array,
                           unsigned int array_length);
const double *player_output(player_t *p, const double *inputs);
unsigned int player_get_fitness(player_t *p);
void player_set_fitness(player_t *p, unsigned int fitness);
void player_print(player_t *p);

#endif /* __PLAYER_H__ */

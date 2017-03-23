#ifndef __POPULATION_H__
#define __POPULATION_H__

#include "player.h"

typedef struct {
    unsigned int num_players;
    unsigned int num_inputs;
    unsigned int num_outputs;
    player_t **players;
    unsigned int num_chromo;
    double *mom_weights;
    double *dad_weights;
    double *child1_weights;
    double *child2_weights;
} population_t;

population_t *population_create(unsigned int num_players,
                                unsigned int num_inputs,
                                unsigned int num_outputs);
void population_destroy(population_t *pop);
void population_natural_selection(population_t *pop);
const double *population_output(population_t *pop, unsigned int player_idx,
                                const double *inputs);
player_t *population_get_player(population_t *pop, unsigned int player_id);
void population_save(population_t *pop, const char *filename);
int population_load(population_t *pop, const char *filename);

#endif /* __POPULATION_H__ */
